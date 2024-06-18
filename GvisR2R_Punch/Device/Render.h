#ifndef __RENDER_H__
#define __RENDER_H__

#include "RGBConvert.h"
#include "Media/VideoRender.h"

class CRender
{
public:

	CRender(HANDLE handler);

	CRender();

	void setHandle(HANDLE handler);

	~CRender();

	bool open();

	bool close();

	bool isOpen() const;

	void setWidth(int iWidth){	m_width = iWidth; }

	void setHeight(int iHeight){ m_height = iHeight; }

	 int getWidth()  { return m_width; }

	 int getHeight() { return m_height; }

	 bool display(unsigned char* displayBuffer, int iWidth, int iHeight, IMV_EPixelType iPixelFormat);
	
private:
	int					m_width;
	int					m_height;
	HANDLE				m_wind;
	VR_HANDLE			m_handler;
	VR_OPEN_PARAM_S		m_params;
};

#endif //__RENDER_H__

