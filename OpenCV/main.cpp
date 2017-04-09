#include <opencv2\opencv.hpp>
#include<vector>
#include<iostream>
#include<fstream>
#include<math.h>
using namespace cv;
using namespace std;

//ʮ�������ַ���תʮ����
uint HexStr2Uint(String hexStr)
{
	uint ret = 0;
	for (int i = 0; i < hexStr.size(); i++)
	{
		char tmp = hexStr[hexStr.size() - i - 1];
		tmp = tmp >= 'a' && tmp << 'z' ? tmp - 32 : tmp;
		ret += (tmp >= '0' && tmp <= '9' ? tmp - '0' : tmp - 'A' + 10)*pow(16, i);
	}
	return ret;
}

//��ȡͼ������ؾ���
vector<vector<CvScalar>> getImageMatrix(IplImage *img)
{
	int width = img->width,
		height = img->height;
	vector<vector<CvScalar>> matrix(height);
	for (int i = 0; i < height; i++)
	{
		matrix[i].resize(width);
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			matrix[i][j] = cvGet2D(img, i, j);

		}
	}
	return matrix;
}

//��ȡCvScalar��ʮ��������ɫ��
String CvScalar2HexColor(CvScalar cvscalar, bool withSharpHead = false)
{
	char buffer[3][3];
	_itoa((int)cvscalar.val[2], buffer[0], 16);
	_itoa((int)cvscalar.val[1], buffer[1], 16);
	_itoa((int)cvscalar.val[0], buffer[2], 16);
	char rgbChar[7];
	sprintf(rgbChar, "%02s%02s%02s", buffer[0], buffer[1], buffer[2]);
	String retStr = withSharpHead ? "#" : "";
	retStr += rgbChar;
	return retStr;
}

//ʮ��������ɫתCvScalar
CvScalar HexColor2CvScalar(String hexColor, bool withSharpHead = false)
{
	hexColor = withSharpHead ? hexColor.substr(1, hexColor.size()) : hexColor;
	CvScalar *cv = new CvScalar
	(
		(double)HexStr2Uint(hexColor.substr(4, 2)),
		(double)HexStr2Uint(hexColor.substr(2, 2)),
		(double)HexStr2Uint(hexColor.substr(0, 2))
	);
	return *cv;
}

//��ȡ��ɫ��ֵ
uint getColorDistance(CvScalar source, CvScalar dist)
{
	return
		abs(source.val[0] - dist.val[0]) +
		abs(source.val[1] - dist.val[1]) +
		abs(source.val[2] - dist.val[2]);
}

//��ȡ�������ɫ�±�
uint getSimColorIndex(vector<CvScalar> colors, CvScalar target) 
{
	vector<CvScalar>::iterator index;
	uint min = UINT_MAX;
	for (vector<CvScalar>::iterator i = colors.begin(); i != colors.end(); i++)
	{
		uint distance = getColorDistance(target, *i);
		if(distance < min)
		{
			index = i;
			min = distance;
		}
	}
	return index - colors.begin();
}

//��ά��ɫ����ת��Ϊ��Ҫ����ɫ������ɵľ���
vector<vector<CvScalar>> D2Vector2SimPixelMatrix(vector<vector<CvScalar>> matrix, vector<CvScalar> placeColors)
{
	vector<vector<CvScalar>> m = vector<vector<CvScalar>>(matrix);
	for (vector<vector<CvScalar>>::iterator iterH = m.begin(); iterH != m.end(); iterH++)
	{
		for (vector<CvScalar>::iterator cvscalar = iterH->begin(); cvscalar != iterH->end(); cvscalar++)
		{
			uint index = getSimColorIndex(placeColors, *cvscalar);
			*cvscalar = placeColors[index];
		}
	}
	return m;
}

//��ά��ɫ����ת��Ϊ��Ҫ����ɫ������ɵ���������
vector<vector<uint>> D2Vector2SimPixelIndexMatrix(vector<vector<CvScalar>> matrix, vector<CvScalar> placeColors)
{
	vector<vector<CvScalar>> m = vector<vector<CvScalar>>(matrix);
	vector<vector<uint>> ret(matrix.size());
	for (int i = 0; i < matrix.size(); i++)
	{
		ret[i].resize(matrix[i].size());
	}
	for (vector<vector<CvScalar>>::iterator iterH = m.begin(); iterH != m.end(); iterH++)
	{
		for (vector<CvScalar>::iterator cvscalar = iterH->begin(); cvscalar != iterH->end(); cvscalar++)
		{
			uint index = getSimColorIndex(placeColors, *cvscalar);
			ret[iterH - m.begin()][cvscalar - iterH->begin()] = index;
		}
	}
	return ret;
}

//������ɫ�����ȡ��Ӧ��CvScalar��vector
vector<CvScalar> getPlaceColors(String colors[], uint total)
{
	vector<CvScalar> placeColors;
	for (int i = 0; i < total; i++)
	{
		placeColors.push_back(HexColor2CvScalar(colors[i], true));
	}
	return placeColors;
}

//���ݶ�άvector����ͼ��
IplImage *createImageFromVector(vector<vector<CvScalar>> matrix)
{
	int height = matrix.size();
	int width = matrix[0].size();
	CvSize cvsize;
	cvsize.width = width;
	cvsize.height = height;
	IplImage *img = NULL;
	img = cvCreateImage(cvsize, 8, 3);
	for (int i = 0; i < matrix.size(); i++)
	{
		for (int j = 0; j < matrix[i].size(); j++)
		{
			cvSet2D(img, i, j, matrix[i][j]);
		}
	}
	return img;
}

//��������ƾ���ΪJS��ά����JSON�ļ�
bool ouputIndexMatrix(String filename, vector<vector<uint>> matrix)
{
	ofstream fout;
	fout.open(filename, ios::out);
	fout << '[' << endl;
	for (vector<vector<uint>>::iterator i = matrix.begin(); i != matrix.end(); i++)
	{
		fout << '[';
		for (vector<uint>::iterator j = i->begin(); j != i->end(); j++)
		{
			if (j != i->begin())
			{
				fout << ',';
			}
			fout << *j;
		}
		fout << "]," << endl;
	}
	fout << ']' << endl;
	fout.close();
	return true;
}

//���ͼƬ���ļ�
bool outputImage2File(String filename, IplImage *img)
{
	int params[2] = { CV_IMWRITE_JPEG_QUALITY,100 };
	cvSaveImage(filename.c_str(), img, params);
	return true;
}

//��ͼ���������
IplImage *ResizeImage(IplImage *img, uint maxWidth = NULL, uint maxHeight = NULL, int interpolation = CV_INTER_CUBIC)
{
	CvSize cvsize;
	int imgWidth = img->width,
		imgHeight = img->height;
	maxWidth = NULL == maxWidth ? imgWidth : maxWidth;
	maxHeight = NULL == maxHeight ? imgHeight : maxHeight;
	if ((double)imgWidth / maxWidth > (double)imgHeight / maxHeight)
	{
		cvsize.width = maxWidth;
		cvsize.height = (int)(imgHeight / ((double)imgWidth / maxWidth));
	}
	else
	{
		cvsize.height = maxHeight;
		cvsize.width = (int)(imgWidth / ((double)imgHeight / maxHeight));
	}
	IplImage *dist = cvCreateImage(cvsize, 8, 3);
	cvResize(img, dist, interpolation);
	return dist;
}

String colors[] = { "#FFFFFF", "#E4E4E4", "#888888", "#222222", "#FFA7D1", "#E50000", "#E59500", "#A06A42", "#E5D900", "#94E044", "#02BE01", "#00D3DD", "#0083C7", "#0000EA", "#CF6EE4", "#820080" };

int main(int argc, char* argv[])
{
	String
		str_src,
		str_dst_json,
		str_dst_img;
	uint
		max_width = NULL,
		max_height = NULL;
	if(argc < 5)
	{
		cout << "error argc! usage: pixelate.exe imagePath dstJsonPath dstImagePath [maxWidth,maxHeight]" << endl;
	}
	else
	{
		str_src = argv[1];
		str_dst_json = argv[2];
		str_dst_img = argv[3];
		max_width = argc > 4 ? atoi(argv[4]) : max_width;
		max_height = argc > 5 ? atoi(argv[5]) : max_height;
	}
	IplImage *src = cvLoadImage(str_src.c_str());

	IplImage *to = ResizeImage(src, max_width, max_height);		//ת��С

	vector<vector<CvScalar>> matrix = getImageMatrix(to);	//��ȡ��ɫ����

	vector<CvScalar> placeColors = getPlaceColors(colors, 16);	//��ȡĿ����ɫ��

	vector<vector<uint>> mindex = D2Vector2SimPixelIndexMatrix(matrix, placeColors);	//��ȡ������ɫ������������
	ouputIndexMatrix(str_dst_json, mindex);					//дJSON�ļ�

	vector<vector<CvScalar>> m = D2Vector2SimPixelMatrix(matrix, placeColors);	//����Ŀ����ɫ���ȡ������ɫ����
	IplImage *dist = createImageFromVector(m);								//��ȡ��Сת����ȡ������ɫ������ͼ��
	outputImage2File(str_dst_img, dist);					//дͼƬ

	cout << "Done!" << endl;
	system("pause");
	return 1;
}