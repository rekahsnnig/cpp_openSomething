#include <iostream>
#include <opencv2/opencv.hpp>

#define COLOR_NUM 256

#define CAMERA

int main (int argc, const char* argv[])
{
    //int width = 640, height = 480;

     //int width = 320, height = 240;

     //int width = 160, height = 120;

    double areaSize;
    #ifndef CAMERA
    cv::VideoCapture capture("movie.mov");  //指定したビデオファイルをオープン
    //ビデオファイルがオープンできたかどうかをチェック
    if (capture.isOpened()==0) {
        printf("Specified video not found\n");
        return -1;
    }
    //フレームの大きさを取得
    int width = (int)capture.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    #else
    //int width = 640, height = 480;
    int width = 1280, height = 720;
    cv::VideoCapture capture(0);
    if (capture.isOpened()==0) {
        printf("Camera not found\n");
        return -1;
    }
    capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
    capture.set(CV_CAP_PROP_FPS,30);
    #endif
    cv::Mat frameImage;
    cv::Mat afterGray_Image = cv::Mat::zeros(cv::Size(width,height), CV_32FC1); ;
    cv::Mat grayImage = cv::Mat::zeros(cv::Size(width,height), CV_32FC1);  //1チャンネル
    cv::Mat Before_grayImage = cv::Mat::zeros(cv::Size(width,height), CV_32FC1);  //1チャンネル

    cv::Mat resultImage(cv::Size(width,height), CV_8UC1);  //1チャンネル
    cv::Mat recImage(cv::Size(width,height), CV_8UC3);  //3チャンネル
    //cv::Mat movingAverage = cv::Mat::zeros(cv::Size(width,height), CV_32FC3);

    cv::namedWindow("Frame");
    cv::moveWindow("Frame", 0, 0);
    cv::namedWindow("Gray");
    cv::moveWindow("Gray", width, 0);
    cv::namedWindow("Result");
   // cv::moveWindow("Result", width, height);

    cv::VideoWriter rec("rec.mpg", CV_FOURCC('P','I','M','1'), 30, recImage.size());

   // std::vector< std::vector< std::vector< <cv::Point> > > line; //軌跡を覚えておくために一次元多い
    std::vector< std::vector<cv::Point> > contours;//輪郭線配列
    std::vector<cv::Vec4i> hierarchy;//階層用配列

    // Before_grayImage = grayImage.clone();

    while (1) {
        capture >> frameImage;
        if (frameImage.data==NULL) break;
       // cv::resize(frameImage,frameImage,cv::Size(),0.5,0.5);

        cv::cvtColor(frameImage, grayImage, CV_BGR2GRAY);
        std::cout << grayImage.size() << std::endl;
        std::cout << Before_grayImage.size() << std::endl;
        //grayImage.convertTo(grayImage,CV_32FC1);
        afterGray_Image = grayImage.clone();
        //printf("hnhhhh\n");
        // cv::imshow("Gray", grayImage);
        // cv::imshow("Grayg", Before_grayImage);
         // cv::waitKey(0);

       // https://developers.cyberagent.co.jp/blog/archives/12666/
        cv::accumulateWeighted(grayImage, Before_grayImage, 0.0001,cv::Mat());
        cv::convertScaleAbs(Before_grayImage,Before_grayImage);
        cv::absdiff(grayImage,Before_grayImage,resultImage);

        Before_grayImage = afterGray_Image.clone();
        Before_grayImage.convertTo(Before_grayImage,CV_32FC1);
       // cv::acc(Before_grayImage.rows,Before_grayImage.cols,CV_32F(Before_grayImage.channels()));
        cv::threshold(resultImage,resultImage,5,255,cv::THRESH_BINARY);


        cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3,3));
    cv::dilate(resultImage,resultImage,element,cv::Point(-1,-1),2);
     cv::erode(resultImage,resultImage,element,cv::Point(-1,-1),25);
    // cv::dilate(resultImage,resultImage,element,cv::Point(-1,-1),20);

        cv::Mat tmpImage = resultImage.clone();

        cv::findContours(tmpImage, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


       // line.push_back(contours);
        for(int i = 0;i>=0;i=hierarchy[i][0]){
               //  cv::circle(frameImage,vecarray[i],5,cvScalar(i%180,255,255),-1);
            //if (contours[i][i][i]>=0)
            {
                areaSize = cv::contourArea(contours[i])/sizeof(double);
                if (areaSize>1000)
                {
                    cv::drawContours(frameImage, contours, i, cv::Scalar(255,255,255), 5, 8, hierarchy, 0);
                    cv::drawContours(resultImage, contours, i, cv::Scalar(255,255,255), 5, 8, hierarchy, 0);
                }
            }
        }

       // cv::imshow("Frame", frameImage);
       // cv::imshow("Gray", grayImage);
        cv::imshow("Result", resultImage);

        //(e)動画ファイル書き出し
        cv::cvtColor(resultImage, frameImage,CV_GRAY2BGR);  //動画用3チャンネル画像生成
       // rec << recImage;  //ビデオライタに画像出力
        rec << frameImage;
        //(f)キー入力待ち
        char key = cv::waitKey(1);  //30ミリ秒待機
        if (key=='q') break;
    }
    
    return 0;
}
