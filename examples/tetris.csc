namespace blocks
    var I={{1,1,1,1}}
    var O={{2,2},{2,2}}
    var J={{3,3,3},{0,0,3}}
    var L={{4,4,4},{4,0,0}}
    var Z={{5,5,0},{0,5,5}}
    var S={{0,6,6},{6,6,0}}
    var T={{7,7,7},{0,7,0}}
end
struct tetris
    var width=10
    var height=20
    var store=7
    var now=new array
    var nowid=7
    var nowx=0
    var nowy=0
    var board=new array
    var score=0
    var lose=false
    var changed=false
    var blockbag={blocks.I,blocks.O,blocks.J,blocks.L,blocks.Z,blocks.S,blocks.T}
    function moveable(x,y,b)
        if y+b.size>height or x<0 or x+b[0].size>width
            return false
        end
        for i=0,i<b.size,i++
            for j=0,j<b[0].size,j++
                if board[i+y][j+x]!=0 and b[i][j]!=0
                    return false
                end
            end
        end
        return true
    end
    function newblock()
        nowid=math.randint(0,699)%7
        now=clone(blockbag[nowid])
        nowx=to_integer((width-now[0].size)/2)
        nowy=0
        if not moveable(nowx,nowy,now)
            lose=true
        end
        changed=false
    end
    function clear()
        for i=0,i<height,i++
            var all=true
            for j=0,j<width,j++
                if board[i][j]==0
                    all=false
                end
            end
            if all
                score++
                for h=0,h<i,h++
                    swap(board[i-h],board[i-h-1])
                end
                board[0]=new array
                board[0][width-1]
            end
        end
    end
    function put()
        for i=0,i<now.size,i++
            for j=0,j<now[0].size,j++
                if now[i][j]!=0
                    board[i+nowy][j+nowx]=now[i][j]
                end
            end
        end
        newblock()
    end
    function holdblock()
        if store==7
            var ls=math.randint(0,699)%7
            if moveable(to_integer((width-blockbag[ls][0].size)/2),0,blockbag[ls])
                store=nowid
                nowid=ls
                now=blockbag[nowid]
                nowx=to_integer((width-now[0].size)/2)
                nowy=0
                changed=true
            end
        else
            if moveable(to_integer((width-blockbag[store][0].size)/2),0,blockbag[store])
                swap(nowid,store)
                now=blockbag[nowid]
                nowx=to_integer((width-now[0].size)/2)
                nowy=0
                changed=true
            end
        end
    end
    function spin()
        var ls=new array
        for i=0,i<now[0].size,i++
            ls[i]=new array
            ls[i][now.size-1]
            for j=0,j<now.size,j++
                ls[i][j]=now[now.size-j-1][i]
            end
        end
        if moveable(nowx,nowy,ls)
            swap(now,ls)
        end
    end
    function drop()
        if moveable(nowx,nowy+1,now)
            nowy++
        else
            put()
            clear()
        end
    end
    function harddrop()
        for i=nowy,i<height,i++
            if not moveable(nowx,i,now)
                break
            end
            nowy=i
        end
        put()
        clear()
    end
    function left()
        if moveable(nowx-1,nowy,now)
            nowx--
        end
    end
    function right()
        if moveable(nowx+1,nowy,now)
            nowx++
        end
    end
    function create()
        board=new array
        lose=false
        for i=0,i<height,i++
            board[i]=new array
            board[i][width-1]
        end
        store=7
        newblock()
    end
end
import darwin
using darwin
var t=new tetris
math.randint(0,1)
load()
fit_drawable()
var draw=get_drawable()
var kb=' '
var time=0
var color={black,blue,yellow,cyan,white,red,green,pink}
loop
    var pause=false
    t.create()
    var time=runtime.time()
    loop
        kb=' '
        while is_kb_hit()
            kb=get_kb_hit()
        end
        if kb=='p'
            pause=not pause
        end
        if not pause
            switch kb
                case 'w'
                    if not t.changed
                        t.holdblock()
                        time=runtime.time()
                    end
                end
                case 'a'
                    t.left()
                end
                case 'd'
                    t.right()
                end
                case 's'
                    t.harddrop()
                time=runtime.time()
                end
                case 'q'
                    t.spin()
                end
            end
        end
        if runtime.time()-time>1000
            if not pause
                t.drop()
            end
            time=runtime.time()
        end
        draw.clear()
        fit_drawable()
        var xspace=to_integer((draw.get_width()-32)/2)
        var yspace=to_integer((draw.get_height()-22)/2)
        for i=0,i<t.width,i++
            for j=0,j<t.height,j++
                if t.board[j][i]!=0
                    draw.draw_pixel(i*2+1+xspace,j+1+yspace,pixel(' ',color[t.board[j][i]],color[t.board[j][i]]))
                    draw.draw_pixel(i*2+2+xspace,j+1+yspace,pixel(' ',color[t.board[j][i]],color[t.board[j][i]]))
                else
                    draw.draw_pixel(i*2+1+xspace,j+1+yspace,pixel('[',white,black))
                    draw.draw_pixel(i*2+2+xspace,j+1+yspace,pixel(']',white,black))
                end
            end
        end
        var dp=0
       for i=t.nowy,i<t.height,i++
             if not t.moveable(t.nowx,i,t.now)
                break
            end
            dp=i
        end
        for i=0,i<t.now[0].size,i++
            for j=0,j<t.now.size,j++
                if t.now[j][i]!=0
                    draw.draw_pixel((i+t.nowx)*2+1+xspace,j+dp+1+yspace,pixel('[',color[t.now[j][i]],color[t.now[j][i]]))
                    draw.draw_pixel((i+t.nowx)*2+2+xspace,j+dp+1+yspace,pixel(']',color[t.now[j][i]],color[t.now[j][i]]))
                end
            end
        end
        for i=0,i<t.now[0].size,i++
            for j=0,j<t.now.size,j++
                if t.now[j][i]!=0
                    draw.draw_pixel((i+t.nowx)*2+1+xspace,j+t.nowy+1+yspace,pixel(' ',color[t.now[j][i]],color[t.now[j][i]]))
                    draw.draw_pixel((i+t.nowx)*2+2+xspace,j+t.nowy+1+yspace,pixel(' ',color[t.now[j][i]],color[t.now[j][i]]))
                end
            end
        end
        if t.store!=7
            for i=0,i<t.blockbag[t.store][0].size,i++
                for j=0,j<t.blockbag[t.store].size,j++
                    if t.blockbag[t.store][j][i]!=0
                        draw.draw_pixel((i+t.width)*2+2+xspace,j+yspace+1,pixel(' ',color[t.blockbag[t.store][j][i]],color[t.blockbag[t.store][j][i]]))
                        draw.draw_pixel((i+t.width)*2+3+xspace,j+yspace+1,pixel(' ',color[t.blockbag[t.store][j][i]],color[t.blockbag[t.store][j][i]]))
                    end
                end
            end
        end
        draw.draw_string(22+xspace,6+yspace,"Score:",pixel(' ',blue,white))
        draw.draw_string(22+xspace,7+yspace,to_string(t.score),pixel(' ',blue,white))
        draw.draw_string(22+xspace,9+yspace,"q to spin",pixel(' ',blue,white))
        draw.draw_string(22+xspace,10+yspace,"a/d to move left/right",pixel(' ',blue,white))
        draw.draw_string(22+xspace,11+yspace,"w to hold",pixel(' ',blue,white))
        draw.draw_string(22+xspace,12+yspace,"s to hard drop",pixel(' ',blue,white))
        draw.draw_string(22+xspace,13+yspace,"p to pause",pixel(' ',blue,white))
        draw.draw_rect(xspace,yspace,22,22,pixel(' ',white,white))
        draw.draw_rect(21+xspace,yspace,10,6,pixel(' ',white,white))
        if pause
            draw.draw_string(9+xspace,10+yspace,"PAUSE",pixel(' ',white,red))
        end
        darwin.update_drawable()
    until t.lose
    var ls=""
    ls=ui.input_box("You lost!","Your score is "+t.score+".Input e to exit.","",true)
until ls=="e" or ls=="E"