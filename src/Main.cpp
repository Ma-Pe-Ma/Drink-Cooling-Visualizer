#include <iostream>

#include "MainLogic.h"
#include "Windowing.h"

std::shared_ptr<MainLogic> mainLogic;
std::shared_ptr<Windowing> windowing;

inline void update() {
	windowing->startFrame();

	mainLogic->update();

	windowing->endFrame();
}

int main() {
	windowing = std::make_shared<Windowing>();
	mainLogic = std::make_shared<MainLogic>();	

	windowing->setMainLogic(mainLogic);
	mainLogic->setWindowing(windowing);

	windowing->initialize();
	mainLogic->initialize();	

	std::chrono::system_clock::time_point prevFrameStart = std::chrono::system_clock::now();

#if defined __EMSCRIPTEN__
	emscripten_set_main_loop_arg((em_arg_callback_func) update, windowing->getGLFWWindow(), 60, 1);
#else
	while (!windowing->shouldStop()) {
		std::chrono::duration<double, std::milli> work_time = std::chrono::system_clock::now() - prevFrameStart;

		if (work_time.count() < 1000.0f / 30) {
			std::chrono::duration<double, std::milli> delta_ms(1000.0f / 30 - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			//std::cout<<"Sleep for: "<<delta_ms_duration.count()<<std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		prevFrameStart = std::chrono::system_clock::now();

		update();
	}
#endif	

	mainLogic->terminate();
	windowing->terminate();

	return 0;
}