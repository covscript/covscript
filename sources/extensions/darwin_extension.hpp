#pragma once
#ifndef CS_DEBUG
#define DARWIN_DISABLE_LOG
#endif
#define DARWIN_FORCE_BUILTIN

#include "../../include/darwin/darwin.hpp"
#include "../cni.hpp"
#include <fstream>

static cs::extension darwin_ext;
static cs::extension darwin_ui_ext;
static cs::extension darwin_drawable_ext;
static cs::extension_t darwin_ui_ext_shared = std::make_shared<cs::extension_holder>(&darwin_ui_ext);
static cs::extension_t darwin_drawable_ext_shared = std::make_shared<cs::extension_holder>(&darwin_drawable_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<std::shared_ptr<darwin::drawable>>()
	{
		return darwin_drawable_ext_shared;
	}
}
namespace darwin_cs_ext {
	using namespace cs;
	using drawable_t=std::shared_ptr<darwin::drawable>;
	darwin::sync_clock clock(30);

// Type Constructor
	var pixel(char ch, darwin::colors fc, darwin::colors bc)
	{
		return var::make_protect<darwin::pixel>(ch, true, false, fc, bc);
	}

	var picture(number w, number h)
	{
		return var::make_protect<drawable_t>(new darwin::picture(w, h));
	}

// Darwin Main Function
	void load()
	{
		darwin::runtime.load("./darwin.module");
	}

	void exit(number code)
	{
		darwin::runtime.exit(code);
	}

	bool is_kb_hit()
	{
		return darwin::runtime.is_kb_hit();
	}

	char get_kb_hit()
	{
		return darwin::runtime.get_kb_hit();
	}

	void fit_drawable()
	{
		darwin::runtime.fit_drawable();
	}

	var get_drawable()
	{
		return var::make<drawable_t>(darwin::runtime.get_drawable(), [](darwin::drawable *) {});
	}

	void update_drawable()
	{
		darwin::runtime.update_drawable();
		clock.sync();
		clock.reset();
	}

	void set_frame_limit(number freq)
	{
		clock.set_freq(freq);
	}

	void set_draw_line_precision(number prec)
	{
		darwin::drawable::draw_line_precision = prec;
	}

// Darwin UI Function
	void message_box(const string &title, const string &message, const string &button)
	{
		std::size_t width = std::max(title.size(), std::max(message.size(), button.size())) + 4;
		darwin::sync_clock c(10);
		while (true) {
			c.reset();
			if (darwin::runtime.is_kb_hit()) {
				darwin::runtime.get_kb_hit();
				break;
			}
			darwin::runtime.fit_drawable();
			std::size_t x(0.5 * (darwin::runtime.get_drawable()->get_width() - width)), y(0.5 * (darwin::runtime.get_drawable()->get_height() - 6));
			darwin::runtime.get_drawable()->clear();
			darwin::runtime.get_drawable()->fill_rect(x, y, width, 6, darwin::pixel(' ', true, false, darwin::colors::white, darwin::colors::white));
			darwin::runtime.get_drawable()->fill_rect(x, y, width, 1, darwin::pixel(' ', true, false, darwin::colors::white, darwin::colors::cyan));
			darwin::runtime.get_drawable()->fill_rect(x + 0.5 * (width - button.size() - 2), y + 4, button.size() + 2, 1, darwin::pixel(' ', true, false, darwin::colors::white, darwin::colors::cyan));
			darwin::runtime.get_drawable()->draw_string(x + 0.5 * (width - title.size()), y, title, darwin::pixel(' ', true, false, darwin::colors::black, darwin::colors::cyan));
			darwin::runtime.get_drawable()->draw_string(x + 0.5 * (width - message.size()), y + 2, message, darwin::pixel(' ', true, false, darwin::colors::black, darwin::colors::white));
			darwin::runtime.get_drawable()->draw_string(x + 0.5 * (width - button.size()), y + 4, button, darwin::pixel(' ', true, false, darwin::colors::white, darwin::colors::cyan));
			darwin::runtime.update_drawable();
			c.sync();
		}
	}

	var input_box(const string &title, const string &message, string buff, bool format)
	{
		darwin::sync_clock c(10);
		while (true) {
			c.reset();
			if (darwin::runtime.is_kb_hit()) {
				char ch = darwin::runtime.get_kb_hit();
				if (ch == '\n' || ch == '\r')
					break;
				else if (ch == '\b' || ch == 127) {
					if (!buff.empty())
						buff.pop_back();
				}
				else
					buff += ch;
			}
			darwin::runtime.fit_drawable();
			std::size_t width = std::max(title.size(), std::max(message.size(), buff.size())) + 4;
			std::size_t x(0.5 * (darwin::runtime.get_drawable()->get_width() - width)), y(0.5 * (darwin::runtime.get_drawable()->get_height() - 6));
			darwin::runtime.get_drawable()->clear();
			darwin::runtime.get_drawable()->fill_rect(x, y, width, 6, darwin::pixel(' ', true, false, darwin::colors::white, darwin::colors::white));
			darwin::runtime.get_drawable()->fill_rect(x, y, width, 1, darwin::pixel(' ', true, false, darwin::colors::white, darwin::colors::cyan));
			darwin::runtime.get_drawable()->fill_rect(x + 2, y + 4, width - 4, 1, darwin::pixel(' ', true, false, darwin::colors::white, darwin::colors::black));
			darwin::runtime.get_drawable()->draw_string(x + 0.5 * (width - title.size()), y, title, darwin::pixel(' ', true, false, darwin::colors::black, darwin::colors::cyan));
			darwin::runtime.get_drawable()->draw_string(x + 0.5 * (width - message.size()), y + 2, message, darwin::pixel(' ', true, false, darwin::colors::black, darwin::colors::white));
			darwin::runtime.get_drawable()->draw_string(x + 2, y + 4, buff, darwin::pixel(' ', true, false, darwin::colors::white, darwin::colors::black));
			darwin::runtime.update_drawable();
			c.sync();
		}
		if (format)
			return parse_value(buff);
		else
			return buff;
	}

// Drawable Function
	void load_from_file(const drawable_t &pic, const string &path)
	{
		std::ifstream infs(path);
		if (!infs.is_open())
			throw lang_error("File is not exist.");
		std::deque<char> buff;
		while (!infs.eof())
			buff.push_back(infs.get());
		darwin::unserial_picture(pic.get(), buff);
	}

	void save_to_file(const drawable_t &pic, const string &path)
	{
		std::ofstream outfs(path);
		if (!outfs.is_open())
			throw lang_error("Write file failed.");
		std::deque<char> buff;
		darwin::serial_picture(pic.get(), buff);
		for (auto &ch:buff)
			outfs.put(ch);
		outfs.flush();
	}

	void clear(const drawable_t &pic)
	{
		pic->clear();
	}

	void fill(const drawable_t &pic, const darwin::pixel &pix)
	{
		pic->fill(pix);
	}

	void resize(const drawable_t &pic, number w, number h)
	{
		pic->resize(w, h);
	}

	number get_height(const drawable_t &pic)
	{
		return pic->get_height();
	}

	number get_width(const drawable_t &pic)
	{
		return pic->get_width();
	}

	darwin::pixel get_pixel(const drawable_t &pic, number x, number y)
	{
		return pic->get_pixel(x, y);
	}

	void draw_pixel(const drawable_t &pic, number x, number y, const darwin::pixel &pix)
	{
		pic->draw_pixel(x, y, pix);
	}

	void draw_line(const drawable_t &pic, number x1, number y1, number x2, number y2, const darwin::pixel &pix)
	{
		pic->draw_line(x1, y1, x2, y2, pix);
	}

	void draw_rect(const drawable_t &pic, number x, number y, number w, number h, const darwin::pixel &pix)
	{
		pic->draw_rect(x, y, w, h, pix);
	}

	void fill_rect(const drawable_t &pic, number x, number y, number w, number h, const darwin::pixel &pix)
	{
		pic->fill_rect(x, y, w, h, pix);
	}

	void draw_triangle(const drawable_t &pic, number x1, number y1, number x2, number y2, number x3, number y3, const darwin::pixel &pix)
	{
		pic->draw_triangle(x1, y1, x2, y2, x3, y3, pix);
	}

	void fill_triangle(const drawable_t &pic, number x1, number y1, number x2, number y2, number x3, number y3, const darwin::pixel &pix)
	{
		pic->fill_triangle(x1, y1, x2, y2, x3, y3, pix);
	}

	void draw_string(const drawable_t &pic, number x, number y, const string &str, const darwin::pixel &pix)
	{
		pic->draw_string(x, y, str, pix);
	}

	void draw_picture(const drawable_t &pic, number x, number y, const drawable_t &p)
	{
		pic->draw_picture(x, y, *p);
	}

	void init()
	{
		// Namespaces
		darwin_ext.add_var("ui", var::make_protect<extension_t>(darwin_ui_ext_shared));
		darwin_ext.add_var("drawable", var::make_protect<extension_t>(darwin_drawable_ext_shared));
		// Colors
		darwin_ext.add_var("black", var::make_constant<darwin::colors>(darwin::colors::black));
		darwin_ext.add_var("white", var::make_constant<darwin::colors>(darwin::colors::white));
		darwin_ext.add_var("red", var::make_constant<darwin::colors>(darwin::colors::red));
		darwin_ext.add_var("green", var::make_constant<darwin::colors>(darwin::colors::green));
		darwin_ext.add_var("blue", var::make_constant<darwin::colors>(darwin::colors::blue));
		darwin_ext.add_var("pink", var::make_constant<darwin::colors>(darwin::colors::pink));
		darwin_ext.add_var("yellow", var::make_constant<darwin::colors>(darwin::colors::yellow));
		darwin_ext.add_var("cyan", var::make_constant<darwin::colors>(darwin::colors::cyan));
		// Type Constructor
		darwin_ext.add_var("pixel", var::make_protect<callable>(cni(pixel), true));
		darwin_ext.add_var("picture", var::make_protect<callable>(cni(picture), true));
		// Darwin Main Function
		darwin_ext.add_var("load", var::make_protect<callable>(cni(load)));
		darwin_ext.add_var("exit", var::make_protect<callable>(cni(exit)));
		darwin_ext.add_var("is_kb_hit", var::make_protect<callable>(cni(is_kb_hit)));
		darwin_ext.add_var("get_kb_hit", var::make_protect<callable>(cni(get_kb_hit)));
		darwin_ext.add_var("fit_drawable", var::make_protect<callable>(cni(fit_drawable)));
		darwin_ext.add_var("get_drawable", var::make_protect<callable>(cni(get_drawable)));
		darwin_ext.add_var("update_drawable", var::make_protect<callable>(cni(update_drawable)));
		darwin_ext.add_var("set_frame_limit", var::make_protect<callable>(cni(set_frame_limit)));
		darwin_ext.add_var("set_draw_line_precision", var::make_protect<callable>(cni(set_draw_line_precision)));
		// Darwin UI Function
		darwin_ui_ext.add_var("message_box", var::make_protect<callable>(cni(message_box)));
		darwin_ui_ext.add_var("input_box", var::make_protect<callable>(cni(input_box)));
		// Drawable Function
		darwin_drawable_ext.add_var("load_from_file", var::make_protect<callable>(cni(load_from_file)));
		darwin_drawable_ext.add_var("save_to_file", var::make_protect<callable>(cni(save_to_file)));
		darwin_drawable_ext.add_var("clear", var::make_protect<callable>(cni(clear)));
		darwin_drawable_ext.add_var("fill", var::make_protect<callable>(cni(fill)));
		darwin_drawable_ext.add_var("resize", var::make_protect<callable>(cni(resize)));
		darwin_drawable_ext.add_var("get_height", var::make_protect<callable>(cni(get_height)));
		darwin_drawable_ext.add_var("get_width", var::make_protect<callable>(cni(get_width)));
		darwin_drawable_ext.add_var("get_pixel", var::make_protect<callable>(cni(get_pixel)));
		darwin_drawable_ext.add_var("draw_pixel", var::make_protect<callable>(cni(draw_pixel)));
		darwin_drawable_ext.add_var("draw_line", var::make_protect<callable>(cni(draw_line)));
		darwin_drawable_ext.add_var("draw_rect", var::make_protect<callable>(cni(draw_rect)));
		darwin_drawable_ext.add_var("fill_rect", var::make_protect<callable>(cni(fill_rect)));
		darwin_drawable_ext.add_var("draw_triangle", var::make_protect<callable>(cni(draw_triangle)));
		darwin_drawable_ext.add_var("fill_triangle", var::make_protect<callable>(cni(fill_triangle)));
		darwin_drawable_ext.add_var("draw_string", var::make_protect<callable>(cni(draw_string)));
		darwin_drawable_ext.add_var("draw_picture", var::make_protect<callable>(cni(draw_picture)));
	}
}
