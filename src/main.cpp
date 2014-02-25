#include "main_window.hpp"

#include <quote/quote.hpp>
#include <quote/win32/message_loop.hpp>

#include <locale>

int run()
{
	std::locale::global(std::locale(""));

	if(!main_window::register_class())
		return 0;

	main_window w;
	if(!w.create(nullptr, L"hyphen"))
		return 0;

	w.show();

	auto fc = win32::make_frame_controller<REDRAW_TIMER, 60>(w);

	BOOL ret;
	MSG msg;
	do{
		if(::PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE)){
			ret = ::GetMessageW(&msg, nullptr, 0, 0);
			if(ret == 0 || ret == -1)
				break;
			if(msg.message == WM_KEYDOWN){
				switch(msg.wParam){
				case VK_DELETE:
					w.delete_file();
					break;
				case VK_LEFT:
				case VK_UP:
					w.prev_file();
					break;
				case VK_RIGHT:
				case VK_DOWN:
					w.next_file();
					break;
				}
			}
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}else{
			fc();
		}
	}while(msg.message != WM_QUIT);

	return msg.wParam;

//	return win32::message_loop(win32::make_frame_controller<REDRAW_TIMER, 30>(w));
}

QUOTE_DEFINE_MAIN
