
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	//��ȡ�ļ�test_yuv420p_320x180.yuv
	FILE* fp_yuv=fopen("test_yuv420p_320x180.yuv","rb");
	//д���ļ�frame.yuv
	FILE* fp_frame=fopen("frame_320x180_out.yuv","wb");
	//�����ڴ��ȡtest_yuv420p_320x180.yuv�ļ��ĵ�һ֡���������ݣ�Y��
	char* buffer_y=(char*)malloc(sizeof(char)*320*180);
	char* buffer_u=(char*)malloc(sizeof(char)*320*180/4);
	char* buffer_v=(char*)malloc(sizeof(char)*320*180/4);

	//��ȡ��������test_yuv420p_320x180.yuv�ĵ�һ֡����buffָ����ڴ�
	for(int i=0;i<30;i++)
	{
		fread(buffer_y,320*180,1,fp_yuv);
		fread(buffer_u,320*180/4,1,fp_yuv);
		fread(buffer_v,320*180/4,1,fp_yuv);

		//for(int i=0;i<320*180/4;i++){
		//	buffer_u[i]=128;
		//	buffer_v[i]=128;
		//}

		memset(buffer_u,128,320*180/4);
		memset(buffer_v,128,320*180/4);

		fwrite(buffer_y,320*180,1,fp_frame);
		fwrite(buffer_u,320*180/4,1,fp_frame);
		fwrite(buffer_v,320*180/4,1,fp_frame);
	}
	//fread(buff,320*180,1,fp_yuv);
	//��buffָ����ڴ�д��frame.yuv
	

	//fprintf��ʾ
	//FILE* fp_demo=fopen("demo.txt","wb");
	//char hello[20]="GuangDianGong";
	//fprintf(fp_demo,"Hello World,%s",hello);
	//fclose(fp_demo);

	free(buffer_y);
	free(buffer_u);
	free(buffer_v);
	fclose(fp_yuv);
	fclose(fp_frame);
	

	return 0;
}
