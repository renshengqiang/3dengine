#ifndef _FRAME_LISTENER_
#define _FRAME_LISTENER_

class ENGINE_EXPORT FrameListener{
	public:
		/**
			��֮֡���ʱ��ֵ����΢��Ϊ��λ
			����false��ʾӦ�ó����д���ֹͣ��Ⱦ
		*/
		virtual bool FrameQueued(long timeSinceLastFrame){(void)timeSinceLastFrame;return true;};
};
#endif