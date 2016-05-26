#include "wx/image.h "
#include "wx/stattext.h"
#include "wx/numdlg.h"
#include "wx/scrolwin.h"+
#include "wx/dir.h"
#include <string.h>
#include <wx/wx.h>
#include <vector>

#define CHECK_PTR(x) if(NULL == (x)) wxMessageBox(wxT("无足够内存"), wxT("错误"))
using namespace std;

struct myPoint
{
	float	x;
	float	y;
};

enum
{
	ID_ABOUT	=	8888,
	ID_PATH,
	ID_GOTO
};

class mainFrame: public wxFrame
{
public:
	mainFrame(const wxString& title = wxT("Made In China"));

private:
	wxImage		img;
	wxBitmap	bitmap;
	int			wheelRotation;
	int			point_num;
	int			point_index;
	int			pic_num;
	float		whRate;
	float		scaleRate;
	float		w_width;
	float		w_height;
	wxSize		screenSize;
	bool		isImgLoad;
	bool		isCapture;
	bool		isConfLoad;
	bool		isDrawRect;
	wxString	filename;
	wxMemoryDC	memDC;
	myPoint		pt[45];
	wxPoint		dstpt[45];
	//myPoint		box[5];
	//wxPoint		dstbox[5];
	wxRect		rect;
	wxRect		dstRect;
	void*		m_pFaceAlignHandle;
	wxScrolledWindow* scrollWindow;

	wxStatusBar*	status;
	vector<wxString>	jpgFiles;
	vector<wxString>	ptsFiles;
	vector<wxString>	boxFiles;

	wxString dirName;

private:
	void OnInit();
	bool ReadConf();
	void ParsePoint(const char* file);
	void ParseBox(const char* file);
	//用来获取目录下的文件
	void getFiles(wxString path, vector<wxString>& files, const char* fileType);
	//void InitUI();
	void drawLines();
	void SaveFiles();
	DECLARE_EVENT_TABLE();

private:
	void OnMotion(wxMouseEvent& e);
	void OnPaint(wxPaintEvent& e);
	void OnClick(wxCommandEvent& e);
	void OnMouseWheel(wxMouseEvent& e);
	void OnMouse(wxMouseEvent& e);
	void OnClose(wxCloseEvent& e);
	void OnKeyDown(wxKeyEvent& e);
	void ShowFileDialog(wxCommandEvent& e);
	void GotoDialog(wxCommandEvent& e);
	void OnResize();
	//void SaveConf(wxCommandEvent& e);
};