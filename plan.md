# 根治 callable↔arena 循环 + 全量 context 存活假设简化

## 总体结论

三轮重构已完成。所有资源的生命周期以 **context 存活**为前提：函数、结构体方法、类型构造器、fiber、结构体 type identity、结构体成员数据——逃逸后全部 UB，无例外。

## 三次提交

```
cb10bfe  function_store entry-based ownership
         function_ptr 精简为裸指针；函数由 function_store 统一拥有（unique_ptr）；
         contains_callable 全家删除

ee45862  weak_ptr<context_type> 全量清理
         function::mContext / struct_builder::mContext / fiber 全部改裸指针；
         删除 resolve_ctx + teardown_ctx；4 个测试删除

3ca83a3  移除结构体 process pin
         struct_builder::m_process 删除；structure::m_process 改裸指针；
         测试改名（escaped_structure_member_data_usable_after_context_death，后移除）
```

13 文件，-385/+184 行。335/335 测试通过。

## 当前 ownership 模型

```
context_t (shared_ptr<context_type>)            ← 嵌入者持有，运行时根
└─ context_type
   ├─ subcontexts: vector<context_t>            ← 模块导入（共享父 process）
   ├─ compiler: compiler_t (shared_ptr)
   ├─ instance: instance_t (shared_ptr)
   │  ├─ statements: deque<statement_base*>
   │  ├─ m_unit: shared_ptr<compile_unit>       ← 当前程序 token arena
   │  ├─ functions: function_store
   │  │  └─ m_entries: vector<entry{ unique_ptr<function>, var callable }>
   │  └─ storage: domain_manager               ← 变量域栈
   ├─ process: shared_ptr<process_context>
   │  ├─ type_nodes: deque<type_node>           ← 类型身份（只增不减）
   │  ├─ stack: stack_type<var>                 ← 值栈
   │  └─ fiber_cxt: fiber_context*
   └─ current_unit: shared_ptr<compile_unit>    ← 编译期当前 arena
```

**关键前提**：import 子 context 共享父 process（`create_subcontext` 里 `context->process = cxt->process`），不是 fork。只有 fiber 才 fork 新 process。

### 非拥有反向引用（全部裸指针，context-alive 前提）

| 持有者 | 指向 | 职责 |
|--------|------|------|
| `function::mContext` | context | 调用时访问 instance |
| `struct_builder::mContext` | context | do_inherit / operator() 访问 instance |
| `structure::m_process` | process | finalizer 激活（`run_finalize` 里 `process_run_scope`） |
| `fiber_function::context` | context | 异步 fiber 访问 instance |
| `fiber::cs_context` | context | swap_in/out + 脚本/原生 fiber 区分 |
| `function_ptr::fptr` | function | callable 调用 |

### 逃逸行为总结

| 对象 | context 存活时 | context 销毁后 |
|------|--------------|---------------|
| 函数 / lambda | 正常调用 | UB |
| 结构体方法 | 正常调用 | UB |
| type_t 构造 | 正常构造 | UB |
| 结构体 type identity | 正常 | UB（type_node 在 process 里，不再 pin） |
| 结构体成员数据 | 正常 | UB（文档不再承诺逃逸后可用） |
| fiber | 正常 resume | UB |
| 自包含值数据 | 正常 | UB（不保证逃逸后可用） |

## 简化评估结论

| 候选 | 结论 | 理由 |
|------|------|------|
| `entry.callable`（命名函数为空） | 不动 | 已拍板"空 callable 不碍事"，统一 entry 更简洁 |
| `mContext`（context*）改 instance* | 不动 | 省一层间接，但 context 更"根"，收益极小 |
| `current_unit` 与 `instance::m_unit` 合并 | 不动 | 职责不同：前者是"编译期当前 arena"（import 嵌套切换），后者是"实例的程序" |
| `function::m_unit` 改由 store 集中持 arena | 不动 | 省 N 个 shared_ptr 但引入索引+集中管理，是内存优化非简化 |
| ~~`struct_builder::m_process` 重量级 pin~~ | **已做** | 已在 3ca83a3 中移除；structure::m_process 改裸指针 |

### 遗留问题（已评估，暂不处理）

**命名函数 + arena 双重累积**：命名函数在 store 永不释放（直到 context 销毁），每个函数还 `m_unit` pin 住其编译期 arena。经评估，累积存在于任何重编译场景（REPL 及热重载），且每个编译单元的 arena 本来就小，累积量有限。"每函数独立 arena"优化收益 ≈ 0，不值得做。

## 文档状态

`docs/SDK.md` + `docs/SDK-zh.md` 已随三次提交同步更新：

- ownership 模型表：`the instance's function store`
- 所有权段落：全部反向引用为裸指针，逃逸 = UB
- Section 1：函数/lambda、结构体方法、type_t、结构体成员数据统一 UB，无例外
- Section 4：callable 持非拥有指针，逃逸 UB
- Section 5："escaped" → "store 持有"
- 迁移指南：清理过时的 "arena↔function cycle" 理由
