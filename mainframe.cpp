#include "frame.h"
#include "libParseFace.h"
//#include <tinyxml/tinyxml.h>
#include <fstream>

#include "highgui/highgui.hpp"
#include "imgproc/imgproc_c.h"



#pragma comment(lib, "opencv_core249")
#pragma comment(lib, "opencv_highgui249")
#pragma comment(lib, "opencv_imgproc249")


using namespace std;

BEGIN_EVENT_TABLE(mainFrame, wxFrame)
	EVT_PAINT(mainFrame::OnPaint)
	EVT_MOUSEWHEEL(mainFrame::OnMouseWheel)
	EVT_MOUSE_EVENTS(mainFrame::OnMouse)
	EVT_CLOSE(mainFrame::OnClose)
	EVT_KEY_DOWN(mainFrame::OnKeyDown)
	EVT_MENU(ID_PATH, mainFrame::ShowFileDialog)
	EVT_MENU(ID_GOTO, mainFrame::GotoDialog)
	//EVT_MENU(wxID_EXIT, mainFrame::SaveConf)
END_EVENT_TABLE()

mainFrame::mainFrame(const wxString& title /* = wxT */)
	:wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
	wxImage::AddHandler(new wxJPEGHandler);//添加handler以操作图片
	m_pFaceAlignHandle	=	ParseFace_init();
	OnInit();

	wxMenu* fileMenu = new wxMenu;
	fileMenu->Append(ID_PATH, wxT("路径\tCtrl+F"));
	fileMenu->Append(ID_GOTO, wxT("Goto\tCtrl+G"));
	//wxMenu* fileGoto = new wxMenu;
	//fileGoto->Append(ID_GOTO, wxT("Goto"));

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(fileMenu, wxT("文件"));
	//menuBar->Append(fileGoto, wxT("hh"));
	SetMenuBar(menuBar);
	status	=	CreateStatusBar(1);
	status->SetLabelText(wxT("请选择文件路径"));

	//scrollWindow	=	new wxScrolledWindow(this);
	//scrollWindow->SetScrollbars(10, 10, w_width / 10, w_height / 10);
	//scrollWindow->Scroll(50, 10);
	//int pixelPerUnixX	=	10;
	//int pixelPerUnixY	=	10;
	//int noUnitsX		=	1000;
	//int noUnitsY		=	1000;
	//this->SetScrollbar(wxVSCROLL | wxHSCROLL, 10, 10, noUnitsY);
	//this->SetScrollPos()
	//this->SetScrollbars(pixelPerUnixX, pixelPerUnixY, noUnitsX, noUnitsY);


	Centre();
}

void mainFrame::GotoDialog(wxCommandEvent& e)
{
	if (!isImgLoad)
	{
		wxMessageBox("图片未加载");
		return;
	}
	wxNumberEntryDialog dialog(this, wxT("请选择图片序号"),wxT("Enter a Number: "),
		wxT("Numeric Input"), 0, 0, 99999999);
	if (!isImgLoad)
	{
		return;
	}
	if (wxID_OK == dialog.ShowModal())
	{
		SaveFiles();
		int temp		=	pic_num;
		pic_num			=	dialog.GetValue();
		int n			=	jpgFiles.size() - 1;
		if (pic_num > n)
		{
			wxMessageBox("图片序号超过最大图片数");
			pic_num	=	temp;
			return;
		}
		char t_buf[32];
		sprintf(t_buf, "第%d张", pic_num);
		wxString st(t_buf);
		status->SetLabelText(st);
		filename	=	jpgFiles.at(pic_num);
		img.LoadFile(filename, wxBITMAP_TYPE_JPEG);
		ParsePoint(ptsFiles.at(pic_num).ToStdString().c_str());
		ParseBox(boxFiles.at(pic_num).ToStdString().c_str());
		wxBitmap	temp_bmp(img);
		bitmap		=	temp_bmp;

		int width	=	img.GetWidth();
		int height	=	img.GetHeight();
		whRate		=	(float)width / (float)height;
		w_width		=	width * scaleRate;
		w_height	=	height * scaleRate;
		SetClientSize(w_width, w_height);
		//dc.DrawBitmap(bitmap, 0, 0);
		if (img.IsOk())
		{
			wxImage curret_image	=	img.Scale(w_width, w_height);
			wxBitmap				bmp(curret_image);
			//bitmap				=	&bmp;
			memDC.SelectObject(bmp);
			memDC.SetPen(*wxRED);
			for (int i=0; i<point_num; i++)
			{
				dstpt[i].x	=	scaleRate * pt[i].x;
				dstpt[i].y	=	scaleRate * pt[i].y;
				memDC.DrawCircle(dstpt[i].x, dstpt[i].y, 3);
			}
			drawLines();
			memDC.SetPen(*wxGREEN);
			dstRect.x	=	rect.x	*	scaleRate;
			dstRect.y	=	rect.y	*	scaleRate;
			dstRect.height	=	rect.height * scaleRate;
			dstRect.width	=	rect.width * scaleRate;
			memDC.DrawRectangle(dstRect);
			wxClientDC dc(this);
			dc.Blit(0, 0, screenSize.x, screenSize.y, &memDC, 0, 0, wxCOPY, true);
			//bitmap	=	memDC.GetSelectedBitmap();
			//bitmap.SaveFile("tt.bmp", wxBITMAP_TYPE_BMP);
			isImgLoad	=	true;
		}
		else
			wxMessageBox(wxT("faile to load"));
	}
}

void mainFrame::ShowFileDialog(wxCommandEvent& e)
{
	wxDirDialog f_dialog(this);
	if (wxID_OK != f_dialog.ShowModal())
	{
		return;
	}
	dirName		=	f_dialog.GetPath();
	if (!jpgFiles.empty())
	{
		jpgFiles.clear();
		ptsFiles.clear();
		boxFiles.clear();
		pic_num	=	0;
	}
	getFiles(dirName, jpgFiles, "*.jpg");
	getFiles(dirName, ptsFiles, "*.45pts");
	getFiles(dirName, boxFiles, "*.bbox");

	ParsePoint(ptsFiles.at(0).ToStdString().c_str());
	ParseBox(boxFiles.at(0).ToStdString().c_str());

	isConfLoad	=	true;
	filename	=	jpgFiles.at(0);//
	bool ok		=	img.LoadFile(filename, wxBITMAP_TYPE_JPEG);
	wxBitmap	temp_bmp(img);
	bitmap		=	temp_bmp;

	int width	=	img.GetWidth();
	int height	=	img.GetHeight();
	whRate		=	(float)width / (float)height;
	w_width		=	width;
	w_height	=	height;
	SetClientSize(w_width, w_height);
	//dc.DrawBitmap(bitmap, 0, 0);
	if (img.IsOk())
	{
		//wxImage curret_image	=	img.Scale(w_width, w_height);
		wxBitmap				bmp(img);
		//bitmap				=	&bmp;
		memDC.SelectObject(bmp);
		memDC.SetPen(*wxRED);
		for (int i=0; i<point_num; i++)
		{
			memDC.DrawCircle(dstpt[i].x, dstpt[i].y, 3);
		}
		drawLines();
		memDC.SetPen(*wxGREEN);
		memDC.DrawRectangle(dstRect);

		wxClientDC dc(this);
		dc.Blit(0, 0, screenSize.x, screenSize.y, &memDC, 0, 0, wxCOPY, true);
		isImgLoad	=	true;
		status->SetLabelText(wxT("第0张"));
	}
	else
		wxMessageBox(wxT("faile to load"));
}

void mainFrame::OnInit()
{
	//wxString s(f, wxConvUTF8);
	//filename		=	s.Clone();
	wheelRotation	=	0;
	point_num		=	0;
	point_index		=	0;
	pic_num			=	0;
	isImgLoad		=	false;
	isCapture		=	false;
	isConfLoad		=	false;
	isDrawRect		=	false;
	scaleRate		=	1;
	//pt				=	new point[pt_num];
	//pt[0].x			=	400;
	//pt[0].y			=	300;
	memDC.SetPen(*wxRED_PEN);
	memDC.SetBrush(*wxTRANSPARENT_BRUSH);
	screenSize		=	wxGetDisplaySize();
}
//
//bool mainFrame::ReadConf()
//{
//	TiXmlDocument xml;
//	std::vector<wxPoint> point_vector;
//	if (xml.LoadFile("pt.conf"))
//	{
//		TiXmlElement* root	=	xml.RootElement();
//		CHECK_PTR(root);
//		if (std::string("ptConf") == root->Value())
//		{
//			for (TiXmlNode* nod = root->FirstChild(); nod; nod = nod->NextSibling())
//			{
//				if (std::string("path") == nod->Value())
//				{
//					wxString s(((TiXmlElement*)nod)->GetText());
//					filename	=	s.Clone();
//				}
//				else if (std::string("point") == nod->Value())
//				{
//					wxPoint temp_pt;
//					TiXmlNode* x	=	nod->FirstChild();
//					TiXmlNode* y	=	nod->LastChild();
//					temp_pt.x		=	atoi(((TiXmlElement*)x)->GetText());
//					temp_pt.y		=	atoi(((TiXmlElement*)y)->GetText());
//					point_vector.push_back(temp_pt);
//					point_num++;
//				}
//			}
//
//			int n	=	0;
//			for(std::vector<wxPoint>::iterator it = point_vector.begin();
//				point_vector.end() != it;
//				it++)
//			{
//				pt[n].x	= it->x;
//				pt[n].y = it->y;
//				n++;
//			}
//			return true;
//		}
//		else
//			goto INVALID_FORMAT;
//	}
//	//else
//	//	goto INVALID_FORMAT;
//INVALID_FORMAT:
//	wxMessageBox(wxT("配置文件错误"), wxT("错误"));
//	return false;
////}

void mainFrame::OnClose(wxCloseEvent& e)
{
	if (isConfLoad)
	{
		SaveFiles();
	}
	Destroy();
}

void mainFrame::SaveFiles()
{
	ofstream f;
	//f.open(filename.ToStdString().c_str(), ios::out);
	f.open(ptsFiles.at(pic_num).ToStdString(), ios::out);
	char buf[1024];
	sprintf(buf, "%d\n", point_num);
	f<<buf;
	if (f.is_open())
	{
		for (int i=0; i<point_num; i++)
		{
			sprintf(buf, "%.6f %.6f\n", pt[i].x, pt[i].y);
			f<<buf;
		}
	}
	f.close();

	f.open(boxFiles.at(pic_num).ToStdString(), ios::out);
	sprintf(buf, "bbox:\n%d,%d,%d,%d\n", rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
	f<<buf;
	f.close();
}

void mainFrame::OnPaint(wxPaintEvent& e)
{
	if (!isConfLoad || isImgLoad)
		return;

	//img.LoadFile(filename, wxBITMAP_TYPE_JPEG);
	//wxBitmap	temp_bmp(img);
	//bitmap		=	temp_bmp;

	//int width	=	img.GetWidth();
	//int height	=	img.GetHeight();
	//whRate		=	(float)width / (float)height;
	//w_width		=	width;
	//w_height	=	height;
	//SetClientSize(w_width, w_height);
	////dc.DrawBitmap(bitmap, 0, 0);
	//if (img.IsOk())
	//{
	//	//wxImage curret_image	=	img.Scale(w_width, w_height);
	//	wxBitmap				bmp(img);
	//	//bitmap				=	&bmp;
	//	memDC.SelectObject(bmp);
	//	for (int i=0; i<point_num; i++)
	//	{
	//		memDC.DrawCircle(dstpt[i].x, dstpt[i].y, 3);
	//	}
	//	drawLines();
	//	memDC.SetPen(*wxGREEN);
	//	memDC.DrawRectangle(dstRect);

	//	wxPaintDC dc(this);
	//	//DoPrepareDC(dc);
	//	dc.Blit(0, 0, screenSize.x, screenSize.y, &memDC, 0, 0, wxCOPY, true);
	//	isImgLoad	=	true;
	//}
	//else
	//	wxMessageBox(wxT("faile to load"));
	////InitUI();
}

void mainFrame::OnMouseWheel(wxMouseEvent& e)
{
	if (!isImgLoad)
	{
		wxMessageBox("图片未加载");
		return;
	}
	wheelRotation		=	e.GetWheelRotation();
	int lines			=	wheelRotation / e.GetWheelDelta();
	if (abs(lines) >= 80)
	{
		wheelRotation	=	abs(wheelRotation) / wheelRotation * 80 * e.GetWheelDelta();
		return;
	}
	w_width		+=	lines * 10;
	w_height	=	w_width / whRate;
	scaleRate	=	w_width / (float)bitmap.GetWidth();
	SetClientSize(w_width, w_height);
	wxClientDC	dc(this);
	//img.LoadFile(wxT("E:\\qsy\\MovePointsUI\\test.jpg"), wxBITMAP_TYPE_JPEG);
	dc.SetPen(wxPen(*wxRED, 2, wxSOLID));
	wxImage current_image	=	bitmap.ConvertToImage();
	current_image.Rescale(w_width, w_height);
	//dc.DrawBitmap(curret_image, 0, 0);
	wxBitmap				bmp(current_image);
	//bitmap				=	&bmp;
	memDC.SetPen(*wxRED_PEN);
	//memDC.SetPen(wxPen(*wxRED, (int)scaleRate, wxSOLID));
	memDC.SelectObject(bmp);
	for (int i=0; i<point_num; i++)
	{
		dstpt[i].x	=	scaleRate * pt[i].x;
		dstpt[i].y	=	scaleRate * pt[i].y;
		memDC.DrawCircle(dstpt[i].x, dstpt[i].y, 3);
	}
	//画框
	dstRect.x	=	rect.x	*	scaleRate;
	dstRect.y	=	rect.y	*	scaleRate;
	dstRect.height	=	rect.height * scaleRate;
	dstRect.width	=	rect.width * scaleRate;
	memDC.SetPen(*wxGREEN);
	memDC.DrawRectangle(dstRect);
	drawLines();
	dc.Blit(0, 0, screenSize.x, screenSize.y, &memDC, 0, 0, wxCOPY, true);
	char t_buf[32];
	sprintf(t_buf, "第%d张", pic_num);
	wxString st(t_buf);
	status->SetLabelText(st);
}

void mainFrame::OnMouse(wxMouseEvent& e)
{
	if (!isCapture && !isDrawRect)
	{
		for (int i=0; i<point_num; i++)
		{
			if (abs(e.GetX() - dstpt[i].x) < 10 && abs(e.GetY() - dstpt[i].y) < 10)
			{
				if (e.LeftDown())
				{
					isCapture	=	true;
					point_index	=	i;
					break;
				}
			}
		}
	}
	//挪动点线
	if (e.Dragging() && isCapture)
	{

		wxImage current_image	=	img.Scale(w_width, w_height);
		wxBitmap				bmp(current_image);
		memDC.SelectObject(bmp);
		pt[point_index].x		= e.GetX() / scaleRate;
		pt[point_index].y		= e.GetY() / scaleRate;
		dstpt[point_index].x	= e.GetX();
		dstpt[point_index].y	= e.GetY();

		//将已产生的点、框画出来
		wxClientDC dc(this);
		memDC.SetPen(*wxRED);
		for (int j=0; j<point_num; j++)
		{
			memDC.DrawCircle(dstpt[j].x, dstpt[j].y, 3);
		}
		drawLines();
		memDC.SetPen(*wxGREEN);
		//memDC.SetBrush(*wxTRANSPARENT_BRUSH);
		memDC.DrawRectangle(dstRect);
		dc.Blit(0, 0, screenSize.x, screenSize.y, &memDC, 0, 0, wxCOPY, true);

	}
	else if (isCapture && e.LeftUp())
	{
		isCapture	=	false;
		point_index	=	0;
	}


	//画框
	else if (e.LeftDown() && !isCapture)
	{
		dstRect.x		=	e.GetX();
		dstRect.y		=	e.GetY();
	}

	else if (e.Dragging())
	{
		if (e.LeftIsDown())
		{
			if (!isImgLoad)
			{
				wxMessageBox("图片未加载");
				return;
			}
			isDrawRect	=	true;
			///准备重绘
			wxImage current_image	=	img.Scale(w_width, w_height);
			wxBitmap				bmp(current_image);
			memDC.SelectObject(bmp);

			wxClientDC dc(this);
			dc.SetPen(wxPen(*wxRED, 2, wxSOLID));

			dstRect.SetWidth(e.GetX() - dstRect.x);
			dstRect.SetHeight(e.GetY() - dstRect.y);

			memDC.SetPen(*wxRED);
			for (int j=0; j<point_num; j++)
			{
				memDC.DrawCircle(dstpt[j].x, dstpt[j].y, 3);
			}
			drawLines();
			//memDC.DrawLines(5, dstbox);

			memDC.SetPen(*wxGREEN);
			//memDC.SetBrush(*wxTRANSPARENT_BRUSH);
			memDC.DrawRectangle(dstRect);
			dc.Blit(0, 0, screenSize.x, screenSize.y, &memDC, 0, 0, wxCOPY, true);
		}
	}
	else if (e.LeftUp())
	{
		///保存当前矩形框
		isDrawRect	=	false;
		wxClientDC dc(this);

		int cx = dstRect.x / scaleRate + dstRect.width / scaleRate / 2;
		int cy = dstRect.y / scaleRate + dstRect.height /scaleRate / 2;
		int size  = __min(dstRect.width / scaleRate + 1, dstRect.height / scaleRate+ 1);

		dstRect.x		=	(cx - size / 2) * scaleRate;
		dstRect.y		=	(cy - size / 2) * scaleRate;
		dstRect.width	=	size * scaleRate;
		dstRect.height	=	size * scaleRate;

		rect.x	=	dstRect.x / scaleRate;
		rect.y	=	dstRect.y / scaleRate;
		rect.width	=	dstRect.width / scaleRate;
		rect.height	=	dstRect.height / scaleRate;
	}


}

void mainFrame::ParsePoint(const char* file)
{
	char buffer[256];  
	fstream outFile;  
	outFile.open(file, ios::in);
	int num	=	0;
	while(!outFile.eof())  
	{  
		outFile.getline(buffer,256,'\n');//getline(char *,int,char) 表示该行字符达到256个或遇到换行就结束  
		if (0 == strcmp("45", buffer))
		{
			point_num	=	45;
			continue;
		}
		else if (0 == strcmp("", buffer))
		{
			break;
		}
		const char *d = " ";
		char *p;
		p = strtok(buffer,d);
		pt[num].x	=	atof(p);
		dstpt[num].x	=	atof(p);
		p=strtok(NULL,d);
		pt[num].y	=	atof(p);
		dstpt[num].y	=	atof(p);
		num++;
	}  
	outFile.close();  
}

void mainFrame::ParseBox(const char* file)
{
	char buffer[256];  
	fstream outFile;  
	outFile.open(file, ios::in);
	while(!outFile.eof())  
	{  
		outFile.getline(buffer,256,'\n');//getline(char *,int,char) 表示该行字符达到256个或遇到换行就结束  
		if (0 == strcmp("bbox:", buffer))
		{
			continue;
		}
		else if (0 == strcmp("", buffer))
		{
			break;
		}
		const char *d = ",";
		char *p;
		p = strtok(buffer,d);
		rect.x	=	atoi(p);
		p=strtok(NULL,d);
		rect.y	=	atoi(p);
		p=strtok(NULL,d);
		rect.width	=	atoi(p) - rect.x + 1;
		p=strtok(NULL,d);
		rect.height	=	atoi(p) - rect.y + 1;
		memcpy(&dstRect, &rect, sizeof(wxRect));
	}
	outFile.close();
}

void mainFrame::drawLines()
{
	wxPoint m1[5], m2[5], y1[7], y2[7], b[5], z[13], c1[3], c2[3];
	for (int i=0; i<45; i++)
	{
		if (i < 5)
		{
			m1[i]		=	dstpt[i];
		}
		else if (5 <= i && i < 10)
		{
			m2[i - 5]	=	dstpt[i];
		}
		else if (10 <= i && i < 16)
		{
			y1[i - 10]	=	dstpt[i];
		}
		else if (16 <= i && i < 22)
		{
			y2[i - 16]	=	dstpt[i];
		}
		else if (22 <= i && i < 27)
		{
			b[i - 22]	=	dstpt[i];
		}
		else if (27 <= i && i < 39)
		{
			z[i - 27]	=	dstpt[i];
		}
		else if (39 <= i && i < 42)
		{
			c1[i - 39]	=	dstpt[i];
		}
		else if (42 <= i && i < 45)
		{
			c2[i - 42]	=	dstpt[i];
		}
	}
	y1[6]	=	y1[0];
	y2[6]	=	y2[0];
	z[12]	=	z[0];
	memDC.SetPen(*wxYELLOW);
	//memDC.SetPen(wxPen(*wxYELLOW, (int)scaleRate, wxSOLID));
	memDC.DrawLines(5, m1);
	memDC.DrawLines(5, m2);
	memDC.DrawLines(7, y1);
	memDC.DrawLines(7, y2);
	memDC.DrawLines(5, b);
	memDC.DrawLines(13, z);
	memDC.DrawLines(3, c1);
	memDC.DrawLines(3, c2);
}

void mainFrame::OnKeyDown(wxKeyEvent& e)
{
	if ("d" == (char*)e.GetUnicodeKey())
	{
		return;
	}
	int a = e.GetKeyCode();
	switch (e.GetKeyCode())
	{
	case 65://A
		{
			if (!isImgLoad)
			{
				wxMessageBox("图片未加载");
				return;
			}
			SaveFiles();
			pic_num--;
			wxClientDC dc(this);
			if (pic_num < 0)
			{
				pic_num = 0;
				return;
			}
			char t_buf[32];
			sprintf(t_buf, "第%d张", pic_num);
			wxString st(t_buf);
			status->SetLabelText(st);
			//wxString temp(jpgFiles.at(pic_num).ToStdString().c_str());
			filename	=	jpgFiles.at(pic_num);
			img.LoadFile(filename, wxBITMAP_TYPE_JPEG);
			ParsePoint(ptsFiles.at(pic_num).ToStdString().c_str());
			ParseBox(boxFiles.at(pic_num).ToStdString().c_str());
			wxBitmap	temp_bmp(img);
			bitmap		=	temp_bmp;

			int width	=	img.GetWidth();
			int height	=	img.GetHeight();
			whRate		=	(float)width / (float)height;
			w_width		=	width * scaleRate;
			w_height	=	height * scaleRate;
			SetClientSize(w_width, w_height);
			//dc.DrawBitmap(bitmap, 0, 0);
			if (img.IsOk())
			{
				wxImage curret_image	=	img.Scale(w_width, w_height);
				wxBitmap				bmp(curret_image);
				//bitmap				=	&bmp;
				memDC.SelectObject(bmp);
				memDC.SetPen(*wxRED);
				for (int i=0; i<point_num; i++)
				{
					dstpt[i].x	=	scaleRate * pt[i].x;
					dstpt[i].y	=	scaleRate * pt[i].y;
					memDC.DrawCircle(dstpt[i].x, dstpt[i].y, 3);
				}
				drawLines();
				memDC.SetPen(*wxGREEN);
				dstRect.x	=	rect.x	*	scaleRate;
				dstRect.y	=	rect.y	*	scaleRate;
				dstRect.height	=	rect.height * scaleRate;
				dstRect.width	=	rect.width * scaleRate;

				memDC.DrawRectangle(dstRect);
				dc.Blit(0, 0, screenSize.x, screenSize.y, &memDC, 0, 0, wxCOPY, true);
				//bitmap	=	memDC.GetSelectedBitmap();
				//bitmap.SaveFile("tt.bmp", wxBITMAP_TYPE_BMP);
				isImgLoad	=	true;
			}
			else
				wxMessageBox(wxT("faile to load"));
			return;
		}
	case 68://D
		{
			if (!isImgLoad)
			{
				wxMessageBox("图片未加载");
				return;
			}
			SaveFiles();
			pic_num++;

			wxClientDC dc(this);
			if (pic_num > jpgFiles.size() - 1)
			{
				pic_num = jpgFiles.size() - 1;
				return;
			}
			char t_buf[32];
			sprintf(t_buf, "第%d张", pic_num);
			wxString st(t_buf);
			status->SetLabelText(st);
			//wxString temp(jpgFiles.at(pic_num).c_str());
			filename	=	jpgFiles.at(pic_num);
			img.LoadFile(filename, wxBITMAP_TYPE_JPEG);
			ParsePoint(ptsFiles.at(pic_num).ToStdString().c_str());
			ParseBox(boxFiles.at(pic_num).ToStdString().c_str());

			wxBitmap	temp_bmp(img);
			bitmap		=	temp_bmp;

			int width	=	img.GetWidth();
			int height	=	img.GetHeight();
			whRate		=	(float)width / (float)height;
			w_width		=	width * scaleRate;
			w_height	=	height * scaleRate;
			SetClientSize(w_width, w_height);
			//dc.DrawBitmap(bitmap, 0, 0);
			if (img.IsOk())
			{
				//wxImage curret_image	=	img.Scale(w_width, w_height);
				wxImage	current_image	=	img.Scale(w_width, w_height);
				wxBitmap				bmp(current_image);
				memDC.SelectObject(bmp);
				memDC.SetPen(*wxRED);
				for (int i=0; i<point_num; i++)
				{
					dstpt[i].x	=	scaleRate * pt[i].x;
					dstpt[i].y	=	scaleRate * pt[i].y;
					memDC.DrawCircle(dstpt[i].x, dstpt[i].y, 3);
				}
				drawLines();
				memDC.SetPen(*wxGREEN);
				dstRect.x	=	rect.x	*	scaleRate;
				dstRect.y	=	rect.y	*	scaleRate;
				dstRect.height	=	rect.height * scaleRate;
				dstRect.width	=	rect.width * scaleRate;
				memDC.DrawRectangle(dstRect);
				dc.Blit(0, 0, screenSize.x, screenSize.y, &memDC, 0, 0, wxCOPY, true);
				//bitmap	=	memDC.GetSelectedBitmap();
				//bitmap.SaveFile("tt.bmp", wxBITMAP_TYPE_BMP);
				isImgLoad	=	true;
			}
			else
				wxMessageBox(wxT("faile to load"));
			return;
		}
	case 32://空格键
		{
			if (!isImgLoad)
			{
				wxMessageBox("图片未加载");
				return;
			}
			int cx = dstRect.x / scaleRate + dstRect.width / scaleRate / 2;
			int cy = dstRect.y / scaleRate + dstRect.height /scaleRate / 2;
			int size  = __min(dstRect.width / scaleRate + 1, dstRect.height / scaleRate+ 1);

			dstRect.x		=	(cx - size / 2) * scaleRate;
			dstRect.y		=	(cy - size / 2) * scaleRate;
			dstRect.width	=	size * scaleRate;
			dstRect.height	=	size * scaleRate;

			//特征点识别
			iv_rect face_bbox;

			face_bbox.left   = cx - size / 2;
			face_bbox.right  = cx + size / 2;
			face_bbox.top    = cy - size / 2;
			face_bbox.bottom = cy + size / 2;

			iv_point_64d *palign_landmarks = NULL;
			wxClientDC dc(this);

#if 1
			IplImage *pimg = cvLoadImage(filename.ToStdString().c_str(), 0);
#else

			IplImage *pimg = cvCreateImage(cvSize(640,480), 8, 1);

			unsigned char *pdata = img.GetData();

			for (int i = 0; i < 480; ++i)
			{
				for (int j = 0; j < 640; ++j)
				{
					((uchar*)pimg->imageData + i * pimg->widthStep)[3 * j]     = pdata[i * 640 * 3 + 3 * j + 2];
					((uchar*)pimg->imageData + i * pimg->widthStep)[3 * j + 1] = pdata[i * 640 * 3 + 3 * j + 1]	;				
					((uchar*)pimg->imageData + i * pimg->widthStep)[3 * j + 2] = pdata[i * 640 * 3 + 3 * j];			;
				}
			}

			cvShowImage("img", pimg);
			cvWaitKey(0);
#endif

			ParseFace_face_align(m_pFaceAlignHandle, (unsigned char*)pimg->imageData, pimg->width, pimg->height, pimg->widthStep, face_bbox, &palign_landmarks);

			cvReleaseImage(&pimg);

			for (int idx = 0; idx < 45; ++idx)
			{
				dstpt[idx].x = palign_landmarks[idx].x * scaleRate;
				dstpt[idx].y = palign_landmarks[idx].y * scaleRate;
			}
			wxImage	current_image	=	img.Scale(w_width, w_height);
			wxBitmap				bmp(current_image);
			memDC.SelectObject(bmp);
			memDC.SetPen(*wxRED);
			for (int i=0; i<point_num; i++)
			{
				memDC.DrawCircle(dstpt[i].x, dstpt[i].y, 3);
				pt[i].x	=	dstpt[i].x / scaleRate;
				pt[i].y	=	dstpt[i].y / scaleRate;
			}
			drawLines();
			memDC.SetPen(*wxGREEN);
			memDC.DrawRectangle(dstRect);
			rect.x	=	dstRect.x / scaleRate;
			rect.y	=	dstRect.y / scaleRate;
			dc.Blit(0, 0, screenSize.x, screenSize.y, &memDC, 0, 0, wxCOPY, true);
		}
	}
}

void mainFrame::getFiles(wxString path, vector<wxString>& files, const char* fileType)
{
	////文件句柄
	//long   hFile   =   0;
	////文件信息
	//struct _finddata_t fileinfo;
	//string p;
	//if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)
	//{
	//	do
	//	{
	//		//如果是目录,迭代之
	//		//如果不是,加入列表
	//		if((fileinfo.attrib &  _A_SUBDIR))
	//		{
	//			if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
	//				getFiles( p.assign(path).append("\\").append(fileinfo.name), files );
	//		}
	//		else
	//		{
	//			files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
	//		}
	//	}while(_findnext(hFile, &fileinfo)  == 0);
	//	_findclose(hFile);
	//}

	wxDir dir(path);
	if (!dir.IsOpened())
	{
		return;
	}
	wxString fname;
	bool cont	=	dir.GetFirst(&fname, wxString(fileType));
	while(cont)
	{
		fname = dirName + wxString("\\") + fname;
		files.push_back(fname);
		cont	=	dir.GetNext(&fname);
	}
	dir.Close();
}