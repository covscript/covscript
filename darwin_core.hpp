#include "./core.hpp"
#include "./darwin/darwin.hpp"
#include <cstdlib>
darwin::pixel pencil;
darwin::sync_clock dclock(30);
void init(){
    cov_basic::storage.add_var("darwin_loadmodule",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
		darwin::runtime.load(args.at(0).val<std::string>());
		return 0;
	}));
    cov_basic::storage.add_var("darwin_fitwindow",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
		darwin::runtime.fit_drawable();
		return 0;
	}));
    cov_basic::storage.add_var("darwin_updatepaint",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
		darwin::runtime.update_drawable();
        dclock.sync();
        dclock.reset();
		return 0;
	}));
    cov_basic::storage.add_var("darwin_clearpaint",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
		darwin::runtime.get_drawable()->clear();
		return 0;
	}));
    cov_basic::storage.add_var("darwin_fillpaint",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
        darwin::runtime.get_drawable()->fill(pencil);
		return 0;
	}));
    cov_basic::storage.add_var("darwin_windowheight",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
		return darwin::runtime.get_drawable()->get_height();
	}));
    cov_basic::storage.add_var("darwin_windowwidth",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
		return darwin::runtime.get_drawable()->get_width();
	}));
    cov_basic::storage.add_var("darwin_drawpoint",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
        darwin::runtime.get_drawable()->draw_pixel(args.at(0).val<cov_basic::number>(),args.at(1).val<cov_basic::number>(),pencil);
		return 0;
	}));
    cov_basic::storage.add_var("darwin_drawline",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
        darwin::runtime.get_drawable()->draw_line(args.at(0).val<cov_basic::number>(),args.at(1).val<cov_basic::number>(),args.at(2).val<cov_basic::number>(),args.at(3).val<cov_basic::number>(),pencil);
		return 0;
	}));
    cov_basic::storage.add_var("darwin_setpencil",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
		char ch=args.at(0).val<std::string>().at(0);
        bool bright=args.at(1).val<bool>();
        bool underline=args.at(2).val<bool>();
        darwin::colors fc,bc;
        switch(int(args.at(3).val<cov_basic::number>()))
        {
            case 0:
                fc=darwin::colors::black;
                break;
            case 1:
                fc=darwin::colors::white;
                break;
            case 2:
                fc=darwin::colors::red;
                break;
            case 3:
                fc=darwin::colors::green;
                break;
            case 4:
                fc=darwin::colors::blue;
                break;
            case 5:
                fc=darwin::colors::pink;
                break;
            case 6:
                fc=darwin::colors::yellow;
                break;
            case 7:
                fc=darwin::colors::cyan;
                break;
        }
        switch(int(args.at(4).val<cov_basic::number>()))
        {
            case 0:
                bc=darwin::colors::black;
                break;
            case 1:
                bc=darwin::colors::white;
                break;
            case 2:
                bc=darwin::colors::red;
                break;
            case 3:
                bc=darwin::colors::green;
                break;
            case 4:
                bc=darwin::colors::blue;
                break;
            case 5:
                bc=darwin::colors::pink;
                break;
            case 6:
                bc=darwin::colors::yellow;
                break;
            case 7:
                bc=darwin::colors::cyan;
                break;
        }
        pencil=darwin::pixel(ch,bright,underline,fc,bc);
		return 0;
	}));
}