#ifndef _FRAME_LISTENER_
#define _FRAME_LISTENER_

class ENGINE_EXPORT FrameListener{
	public:
		/**
			两帧之间的时间值，以微秒为单位
			返回false表示应用程序有错误，停止渲染
		*/
		virtual bool FrameQueued(long timeSinceLastFrame){(void)timeSinceLastFrame;return true;};
};
#endif