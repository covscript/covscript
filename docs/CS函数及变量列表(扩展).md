# Covariant Script函数及变量列表(扩展) #
## 数学扩展 ##
pi	圆周率
e	自然底数
number abs(number)	绝对值
number ln(number)	以e为底的对数
number log10(number)	以10为底的对数
number log(number a,number b)	以a为底b的对数
number sin(number)	正弦
number cos(number)	余弦
number tan(number)	正切
number asin(number)	反正弦
number acos(number)	反余弦
number atan(number)	反正切
number sqrt(number)	开方
number root(number a,number b)	a的b次方根
number pow(number a,number b)	a的b次方
## 文件扩展 ##
read_method	读文件
write_method	写文件
[file] open(string path,[method])	打开一个文件
boolean is_open([file])	判断文件是否打开
boolean eof([file])	判断是否到达文件结尾
string getline([file])	从文件中获取输入（阻塞，非格式化输入）
var read([file],...)	从文件中获取输入（阻塞，格式化输入）
var write([file],...)	向文件中输出内容，仅可输出支持to_string的类型（不换行）
## 图形扩展(Covariant Darwin UCGL实现) ##
black 黑色
white	白色
red	红色
green	绿色
blue	蓝色
pink	粉色
yellow	黄色
cyan	青色
[pixel] pixel(char,[color] front,[color] back)	创建一个像素
[drawable] picture(number width,number height)	创建一幅图片
void load(string path)	加载Darwin功能
void exit(number code)	退出程序并清理资源
boolean is_kb_hit()	判断是否有按键按下
char get_kb_hit()	获取按下的按键
void fit_drawable()	使画布适合当前屏幕大小
[drawable] get_drawable()	获取画布
void update_drawable()	将画布中的内容更新至屏幕上
void set_frame_limit(number fps)	设置帧率
void clear_drawable([drawable])	清空画布
void fill_drawable([drawable],[pixel])	填充画布
void resize_drawable([drawable],number width,number height)	重新设置画布大小
number get_width([drawable])	获取画布宽度
number get_height([drawable])	获取画布高度
void draw_pixel([drawable],nuber x,number y,[pixel])	在画布上画点
void draw_picture([drawable],number x,number y,[drawable])	将一幅图片绘制到画布上
void draw_line([drawable],number x1,number y1,number x2,number y2,[pixel])	在画布上画线
void draw_rect([drawable],number x,number y,number width,number height,[pixel])	在画布上绘制线框
void fill_rect([drawable],number x,number y,number width,number height,[pixel])	在画布上填充矩形
void draw_triangle([drawable],number x1,number y1,number x2,number y2,number x3,number y3,[pixel]) 在画布上绘制三角形
void fill_triangle([drawable],number x1,number y1,number x2,number y2,number x3,number y3,[pixel])	在画布上填充三角形
void draw_string([drawable],number x,number y,string,[pixel])	在画布上绘制文字
void message_box(string title,string message,string button)	弹出一个消息对话框
var input_box(string title,string message,string default,boolean format)	弹出一个输入对话框
