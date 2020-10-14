/**
 * ��򵥵�ͼ�ν������Ƶ������
 * Simplest FFmpeg Player GUI
 *
 * ������ Lei Xiaohua
 * leixiaohua1020@126.com
 * �й���ý��ѧ/���ֵ��Ӽ���
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 */

#include "stdafx.h"
#include "SFFPlayer.h"
#include "SFFPlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define __STDC_CONSTANT_MACROS

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "SDL2/SDL.h"
};



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSFFPlayerDlg �Ի���




CSFFPlayerDlg::CSFFPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSFFPlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSFFPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_URL, m_url);
}

BEGIN_MESSAGE_MAP(CSFFPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_START, &CSFFPlayerDlg::OnBnClickedStart)
	ON_BN_CLICKED(ID_PAUSE, &CSFFPlayerDlg::OnBnClickedPause)
//	ON_BN_CLICKED(ID_CONTINUE, &CSFFPlayerDlg::OnBnClickedContinue)
	ON_BN_CLICKED(ID_STOP, &CSFFPlayerDlg::OnBnClickedStop)
	ON_BN_CLICKED(ID_ABOUT, &CSFFPlayerDlg::OnBnClickedAbout)
	ON_BN_CLICKED(IDCANCEL, &CSFFPlayerDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_FILEBROWSER, &CSFFPlayerDlg::OnBnClickedFilebrowser)
END_MESSAGE_MAP()


// CSFFPlayerDlg ��Ϣ�������

BOOL CSFFPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CSFFPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSFFPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSFFPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)

#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)

int thread_exit=0;
int thread_pause=0;

int sfp_refresh_thread(void *opaque){

	thread_exit=0;
	thread_pause=0;

	while (thread_exit==0) {
		if(!thread_pause){
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);
		}
		SDL_Delay(40);
	}
	//Quit
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);
	thread_exit=0;
	thread_pause=0;
	return 0;
}


int simplest_ffmpeg_player(LPVOID lpParam)
{

	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame,*pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int ret, got_picture;

	//------------SDL----------------
	int screen_w,screen_h;
	SDL_Window *screen; 
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Rect sdlRect;
	SDL_Thread *video_tid;
	SDL_Event event;

	struct SwsContext *img_convert_ctx;
	//===========================================
	//�ļ�·��
	CSFFPlayerDlg *dlg=(CSFFPlayerDlg *)lpParam;
	char filepath[250]={0};
	GetWindowTextA(dlg->m_url,(LPSTR)filepath,250);
	//===========================================

	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();

	if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL)!=0){
		AfxMessageBox("Couldn't open input stream.\n");
		return -1;
	}
	if(avformat_find_stream_info(pFormatCtx,NULL)<0){
		AfxMessageBox("Couldn't find stream information.\n");
		return -1;
	}
	videoindex=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++) 
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
			videoindex=i;
			break;
		}
		if(videoindex==-1){
			AfxMessageBox("Didn't find a video stream.\n");
			return -1;
		}
		pCodecCtx=pFormatCtx->streams[videoindex]->codec;
		pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
		if(pCodec==NULL){
			AfxMessageBox("Codec not found.\n");
			return -1;
		}
		if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
			AfxMessageBox("Could not open codec.\n");
			return -1;
		}
		pFrame=av_frame_alloc();
		pFrameYUV=av_frame_alloc();
		out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
		avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

		img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
			pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 


		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {  
			AfxMessageBox( "Could not initialize SDL\n"); 
			return -1;
		} 
		//SDL 2.0 Support for multiple windows
		screen_w = pCodecCtx->width;
		screen_h = pCodecCtx->height;

		//��ʾ�ڵ�������
		//screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		//	screen_w, screen_h,SDL_WINDOW_OPENGL);
		//===========================================
		//��ʾ��MFC�ؼ���
		screen = SDL_CreateWindowFrom(dlg->GetDlgItem(IDC_SCREEN)->GetSafeHwnd());
		//===========================================
		if(!screen) {  
			AfxMessageBox("SDL: could not create window - exiting\n");
			return -1;
		}
		sdlRenderer = SDL_CreateRenderer(screen, -1, 0);  
		//IYUV: Y + U + V  (3 planes)
		//YV12: Y + V + U  (3 planes)
		sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,pCodecCtx->width,pCodecCtx->height);  

		sdlRect.x=0;
		sdlRect.y=0;
		sdlRect.w=screen_w;
		sdlRect.h=screen_h;

		packet=(AVPacket *)av_malloc(sizeof(AVPacket));

		video_tid = SDL_CreateThread(sfp_refresh_thread,NULL,NULL);
		//------------SDL End------------
		//Event Loop

		for (;;) {
			//Wait
			SDL_WaitEvent(&event);
			if(event.type==SFM_REFRESH_EVENT){
				//------------------------------
				if(av_read_frame(pFormatCtx, packet)>=0){
					if(packet->stream_index==videoindex){
						ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
						if(ret < 0){
							AfxMessageBox("Decode Error.\n");
							return -1;
						}
						if(got_picture){
							sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
							//SDL---------------------------
							SDL_UpdateTexture( sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0] );  
							SDL_RenderClear( sdlRenderer );  
							//SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );  
							SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, NULL);  
							SDL_RenderPresent( sdlRenderer );  
							//SDL End-----------------------
							TRACE("Decode 1 frame\n");
						}
					}
					av_free_packet(packet);
				}else{
					//Exit Thread
					thread_exit=1;
				}
			}else if(event.type==SDL_QUIT){
				thread_exit=1;
			}else if(event.type==SFM_BREAK_EVENT){
				break;
			}

		}

		sws_freeContext(img_convert_ctx);

		SDL_DestroyWindow(screen);
		SDL_Quit();
		//FIX Small Bug
		//SDL Hide Window When it finished
		dlg->GetDlgItem(IDC_SCREEN)->ShowWindow(SW_SHOWNORMAL);
		//--------------
		av_frame_free(&pFrameYUV);
		av_frame_free(&pFrame);
		avcodec_close(pCodecCtx);
		avformat_close_input(&pFormatCtx);

		return 0;
}



UINT Thread_Play(LPVOID lpParam){
	CSFFPlayerDlg *dlg=(CSFFPlayerDlg *)lpParam;
	simplest_ffmpeg_player(lpParam);
	return 0;
}



void CSFFPlayerDlg::OnBnClickedStart()
{
	/*
	char url[250]={0};
	m_url.GetWindowTextA(url,250);
	if(strcmp(url,"")==0){
		AfxMessageBox("�ļ�·��Ϊ�գ�");
		return ;
	}
	*/
	pThreadPlay = AfxBeginThread(Thread_Play,this);//�����߳�
}


void CSFFPlayerDlg::OnBnClickedPause()
{
	thread_pause=!thread_pause;
}


void CSFFPlayerDlg::OnBnClickedStop()
{
	thread_exit=1;
}


void CSFFPlayerDlg::OnBnClickedAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}


void CSFFPlayerDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}


void CSFFPlayerDlg::OnBnClickedFilebrowser()
{
	CString FilePathName;
	CFileDialog dlg(TRUE,NULL,NULL,NULL,NULL);///TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի��� 
	if(dlg.DoModal()==IDOK) {
		FilePathName=dlg.GetPathName();
		m_url.SetWindowText(FilePathName);
	}
}
