#pragma once
#include <cstddef>
#include <fstream>
#include <cmath>
#include <deque>
#include "./core.hpp"
namespace darwin {
	class picture:public drawable {
	protected:
		std::size_t mWidth,mHeight;
		pixel* mImage=nullptr;
		void copy(std::size_t w,std::size_t h,pixel* const img) {
			if(img!=nullptr) {
				delete[] this->mImage;
				this->mImage=new pixel[w*h];
				this->mWidth=w;
				this->mHeight=h;
				for(std::size_t i=0; i<this->mWidth*this->mHeight; ++i)
					this->mImage[i]=img[i];
			}
		}
	public:
		picture():mWidth(0),mHeight(0),mImage(nullptr) {}
		picture(std::size_t w,std::size_t h):mImage(new pixel[h*w]),mWidth(w),mHeight(h) {}
		picture(std::size_t w,std::size_t h,const pixel& pix):mImage(new pixel[h*w]),mWidth(w),mHeight(h) {
			for(pixel* it=this->mImage; it!=this->mImage+w*h; ++it) *it=pix;
		}
		picture(const picture& img):mWidth(0),mHeight(0),mImage(nullptr) {
			copy(img.mWidth,img.mHeight,img.mImage);
		}
		picture(picture&& img):mWidth(0),mHeight(0),mImage(nullptr) {
			copy(img.mWidth,img.mHeight,img.mImage);
		}
		virtual ~picture() {
			delete[] this->mImage;
		}
		picture& operator=(const picture& img) {
			if(&img!=this)
				this->copy(img.mWidth,img.mHeight,img.mImage);
			return *this;
		}
		picture& operator=(picture&& img) {
			if(&img!=this)
				this->copy(img.mWidth,img.mHeight,img.mImage);
			return *this;
		}
		virtual std::shared_ptr<drawable> clone() noexcept override {
			return std::make_shared<picture>(*this);
		}
		virtual bool usable() const noexcept override {
			return this->mImage!=nullptr;
		}
		virtual std::size_t get_width() const override {
			return this->mWidth;
		}
		virtual std::size_t get_height() const override {
			return this->mHeight;
		}
		virtual void resize(std::size_t w,std::size_t h) override {
			if(w==this->mWidth&&h==this->mHeight)
				return;
			delete[] this->mImage;
			this->mImage=new pixel[h*w];
			this->mWidth=w;
			this->mHeight=h;
		}
		virtual void fill(const pixel& pix) override {
			if(this->mImage==nullptr)
				Darwin_Error("Use of not available object.");
			for(pixel* it=this->mImage; it!=this->mImage+this->mWidth*this->mHeight; ++it) *it=pix;
		}
		virtual void clear() override {
			if(this->mImage!=nullptr) {
				delete[] this->mImage;
				this->mImage=new pixel[mHeight*mWidth];
			}
		}
		virtual const pixel& get_pixel(std::size_t x,std::size_t y) const override {
			if(this->mImage==nullptr)
				Darwin_Error("Use of not available object.");
			if(x>this->mWidth-1||y>this->mHeight-1)
				Darwin_Error("Out of range.");
			return this->mImage[y*this->mWidth+x];
		}
		virtual void draw_pixel(int x,int y,const pixel& pix) override {
			if(this->mImage==nullptr)
				Darwin_Error("Use of not available object.");
			if(x<0||y<0||x>this->mWidth-1||y>this->mHeight-1) {
				Darwin_Warning("Out of range.");
				return;
			}
			this->mImage[y*this->mWidth+x]=pix;
		}
	};
	bool serial_picture(drawable* pic,std::deque<char>& dat)
	{
		if(pic==nullptr) return false;
		if(pic->get_width()>9999||pic->get_height()>9999) return false;
		static std::string tmp;
		dat.clear();
		tmp=std::to_string(pic->get_width());
		for(int count=4-tmp.size(); count>0; --count)
			dat.push_back('0');
		for(auto& ch:tmp)
			dat.push_back(ch);
		tmp=std::to_string(pic->get_height());
		for(int count=4-tmp.size(); count>0; --count)
			dat.push_back('0');
		for(auto& ch:tmp)
			dat.push_back(ch);
		for(std::size_t y=0; y<pic->get_height(); ++y) {
			for(std::size_t x=0; x<pic->get_width(); ++x) {
				const pixel& pix=pic->get_pixel(x,y);
				dat.push_back(pix.get_char());
				if(pix.is_bright())
					dat.push_back('0');
				else
					dat.push_back('1');
				if(pix.is_underline())
					dat.push_back('0');
				else
					dat.push_back('1');
				switch(pix.get_front_color()) {
				case colors::white:
					dat.push_back('1');
					break;
				case colors::black:
					dat.push_back('2');
					break;
				case colors::red:
					dat.push_back('3');
					break;
				case colors::green:
					dat.push_back('4');
					break;
				case colors::blue:
					dat.push_back('5');
					break;
				case colors::pink:
					dat.push_back('6');
					break;
				case colors::yellow:
					dat.push_back('7');
					break;
				case colors::cyan:
					dat.push_back('8');
					break;
				}
				switch(pix.get_back_color()) {
				case colors::white:
					dat.push_back('1');
					break;
				case colors::black:
					dat.push_back('2');
					break;
				case colors::red:
					dat.push_back('3');
					break;
				case colors::green:
					dat.push_back('4');
					break;
				case colors::blue:
					dat.push_back('5');
					break;
				case colors::pink:
					dat.push_back('6');
					break;
				case colors::yellow:
					dat.push_back('7');
					break;
				case colors::cyan:
					dat.push_back('8');
					break;
				}
			}
		}
		return true;
	}
	bool unserial_picture(drawable* pic,const std::deque<char>& dat)
	{
		if(pic==nullptr) return false;
		std::string tmp;
		tmp= {dat[0],dat[1],dat[2],dat[3]};
		std::size_t w=std::stoul(tmp);
		tmp= {dat[4],dat[5],dat[6],dat[7]};
		std::size_t h=std::stoul(tmp);
		pic->resize(w,h);
		pixel pix;
		int x(0),y(0);
		for(std::size_t i=8; i<dat.size(); i+=5) {
			pix.set_char(dat[i]);
			if(dat[i+1]=='0')
				pix.set_bright(true);
			else
				pix.set_bright(false);
			if(dat[i+2]=='0')
				pix.set_underline(true);
			else
				pix.set_underline(false);
			switch(dat[i+3]) {
			case '1':
				pix.set_front_color(colors::white);
				break;
			case '2':
				pix.set_front_color(colors::black);
				break;
			case '3':
				pix.set_front_color(colors::red);
				break;
			case '4':
				pix.set_front_color(colors::green);
				break;
			case '5':
				pix.set_front_color(colors::blue);
				break;
			case '6':
				pix.set_front_color(colors::pink);
				break;
			case '7':
				pix.set_front_color(colors::yellow);
				break;
			case '8':
				pix.set_front_color(colors::cyan);
				break;
			}
			switch(dat[i+4]) {
			case '1':
				pix.set_back_color(colors::white);
				break;
			case '2':
				pix.set_back_color(colors::black);
				break;
			case '3':
				pix.set_back_color(colors::red);
				break;
			case '4':
				pix.set_back_color(colors::green);
				break;
			case '5':
				pix.set_back_color(colors::blue);
				break;
			case '6':
				pix.set_back_color(colors::pink);
				break;
			case '7':
				pix.set_back_color(colors::yellow);
				break;
			case '8':
				pix.set_back_color(colors::cyan);
				break;
			}
			pic->draw_pixel(x,y,pix);
			if(x==w-1) {
				x=0;
				++y;
			} else
				++x;
		}
		return true;
	}
}