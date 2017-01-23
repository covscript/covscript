#pragma once
#include <array>
#include <cmath>
#include <string>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include "./debugger.hpp"
namespace darwin {
	enum class status {
	    null,ready,busy,leisure,error
	};
	enum class results {
	    null,success,failure
	};
	enum class colors {
	    white,black,red,green,blue,pink,yellow,cyan
	};
	enum class attris {
	    bright,underline
	};
	enum class commands {
	    echo_on,echo_off,reset_cursor,reset_attri,clrscr
	};
	class pixel final {
		char mChar=' ';
		std::array<bool,2> mAttris= {{false,false}};
		std::array<colors,2> mColors= {{colors::white,colors::black}};
	public:
		pixel()=default;
		pixel(const pixel&)=default;
		pixel(char ch,bool bright,bool underline,colors fcolor,colors bcolor):mChar(ch),mAttris( {
			bright,underline
		}),mColors({fcolor,bcolor}) {}
		~pixel()=default;
		void set_char(char c) {
			mChar=c;
		}
		char get_char() const {
			return mChar;
		}
		void set_front_color(colors c) {
			mColors[0]=c;
		}
		colors get_front_color() const {
			return mColors[0];
		}
		void set_back_color(colors c) {
			mColors[1]=c;
		}
		colors get_back_color() const {
			return mColors[1];
		}
		void set_colors(const std::array<colors,2>& c) {
			mColors=c;
		}
		const std::array<colors,2>& get_colors() const {
			return mColors;
		}
		void set_bright(bool value) {
			mAttris[0]=value;
		}
		bool is_bright() const {
			return mAttris[0];
		}
		void set_underline(bool value) {
			mAttris[1]=value;
		}
		bool is_underline() const {
			return mAttris[1];
		}
		void set_attris(const std::array<bool,2>& a) {
			mAttris=a;
		}
		const std::array<bool,2>& get_attris() const {
			return mAttris;
		}
	};
	class drawable {
	public:
		drawable()=default;
		drawable(const drawable&)=default;
		virtual ~drawable()=default;
		virtual std::type_index get_type() const final {
			return typeid(*this);
		}
		virtual std::shared_ptr<drawable> clone() noexcept {
			return nullptr;
		}
		virtual bool usable() const noexcept=0;
		virtual void clear()=0;
		virtual void fill(const pixel&)=0;
		virtual void resize(std::size_t,std::size_t)=0;
		virtual std::size_t get_width() const=0;
		virtual std::size_t get_height() const=0;
		virtual const pixel& get_pixel(std::size_t,std::size_t) const=0;
		virtual void draw_pixel(int,int,const pixel&)=0;
		virtual void draw_line(int p0x,int p0y,int p1x,int p1y,const pixel& pix) {
			if(!this->usable())
				Darwin_Error("Use of not available object.");
			if(p0x<0||p0y<0||p1x<0||p1y<0||p0x>this->get_width()-1||p0y>this->get_height()-1||p1x>this->get_width()-1||p1y>this->get_height()-1)
				Darwin_Warning("Out of range.");
			long w(p1x-p0x),h(p1y-p0y);
			double distance(std::sqrt(std::pow(w,2)+std::pow(h,2)));
			for(double c=0; c<=1; c+=1.0/distance)
				this->draw_pixel(static_cast<int>(p0x+w*c),static_cast<int>(p0y+h*c),pix);
			this->draw_pixel(p0x,p0y,pix);
			this->draw_pixel(p1x,p1y,pix);
		}
		virtual void draw_picture(int col,int row,const drawable& pic) {
			if(!this->usable()||!pic.usable())
				Darwin_Error("Use of not available object.");
			if(col<0||row<0||col>this->get_width()-1||row>this->get_height()-1)
				Darwin_Warning("Out of range.");
			int y0(row>=0?row:0),y1(row>=0?0:-row);
			while(y0<this->get_height()&&y1<pic.get_height()) {
				int x0(col>=0?col:0),x1(col>=0?0:-col);
				while(x0<this->get_width()&&x1<pic.get_width()) {
					this->draw_pixel(x0,y0,pic.get_pixel(x1,y1));
					++x0;
					++x1;
				}
				++y0;
				++y1;
			}
		}
	};
}