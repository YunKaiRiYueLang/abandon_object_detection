/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"detectabandon.h"

namespace bgslibrary
{
  //template<typename T> IBGS* createInstance() { return new T; }
  //typedef std::map<std::string, IBGS*(*)()> map_ibgs;

  IBGS* get_alg(std::string alg_name) {
    map_ibgs map;

    map["LBFuzzyGaussian"] = &createInstance<LBFuzzyGaussian>;

    return map[alg_name]();
  }

  QStringList get_algs_name()
  {
    QStringList stringList;

    stringList.append("LBFuzzyGaussian");

    return stringList;
  }
}

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  //QDir applicationPath(QCoreApplication::applicationDirPath());
  fileName = QDir(".").filePath("dataset/video.avi");
  //fileName = applicationPath.absolutePath() + "dataset";
  ui->lineEdit_inputdata->setText(fileName);
  //fileName = ui->lineEdit_inputdata->text();
  timer = new QTimer(this); connect(timer, SIGNAL(timeout()), this, SLOT(startCapture()));
  QStringListModel* listModel = new QStringListModel(bgslibrary::get_algs_name(), NULL);/*初始化方法列表种的值*/
  listModel->sort(0);
  ui->listView_algorithms->setModel(listModel);
  QModelIndex index = listModel->index(0);
  ui->listView_algorithms->selectionModel()->select(index, QItemSelectionModel::Select);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_actionExit_triggered()
{
  this->close();
}

void MainWindow::on_pushButton_inputdata_clicked()
{
  QFileDialog dialog(this);

  if (ui->checkBox_imageseq->isChecked())
    dialog.setFileMode(QFileDialog::Directory);
  else
    dialog.setFileMode(QFileDialog::ExistingFile);
  //dialog.setFileMode(QFileDialog::AnyFile);

  dialog.exec();
  QStringList list = dialog.selectedFiles();
  /*
    for(int index = 0; index < list.length(); index++)
       std::cout << list.at(index).toStdString() << std::endl;
    */
  if (list.size() > 0)
  {
    fileName = list.at(0);
    ui->lineEdit_inputdata->setText(fileName);
  }
}

void MainWindow::on_pushButton_out_in_clicked()
{
  QFileDialog dialog(this);
  dialog.setDirectory(".");
  dialog.setFileMode(QFileDialog::Directory);
  dialog.exec();
  QStringList list = dialog.selectedFiles();
  if (list.size() > 0)
  {
    fileName = list.at(0);
    ui->lineEdit_out_in->setText(fileName);
  }
}

void MainWindow::on_pushButton_out_fg_clicked()
{
  QFileDialog dialog(this);
  dialog.setDirectory(".");
  dialog.setFileMode(QFileDialog::Directory);
  dialog.exec();
  QStringList list = dialog.selectedFiles();
  if (list.size() > 0)
  {
    fileName = list.at(0);
    ui->lineEdit_out_fg->setText(fileName);
  }
}

void MainWindow::on_pushButton_out_bg_clicked()
{
  QFileDialog dialog(this);
  dialog.setDirectory(".");
  dialog.setFileMode(QFileDialog::Directory);
  dialog.exec();
  QStringList list = dialog.selectedFiles();
  if (list.size() > 0)
  {
    fileName = list.at(0);
    ui->lineEdit_out_bg->setText(fileName);
  }
}

void MainWindow::on_pushButton_start_clicked()
{
   inittracker(track);
  useCamera = false;
  useVideo = false;
  useSequence = false;

  if (ui->checkBox_webcamera->isChecked())
    useCamera = true;
  else
  {
    if (ui->checkBox_imageseq->isChecked())
      useSequence = true;
    else
      useVideo = true;
  }

  if (!timer->isActive() && setUpCapture())
  {
    createBGS();
    startTimer();
  }
}

void MainWindow::on_pushButton_stop_clicked()
{
  stopTimer();
}

void MainWindow::createBGS()
{
  QString algorithm_name = getSelectedAlgorithmName();
  bgs = bgslibrary::get_alg(algorithm_name.toStdString());
  bgs->setShowOutput(false);
}

void MainWindow::destroyBGS()
{
  delete bgs;
}

void MainWindow::startTimer()
{
  std::cout << "startTimer()" << std::endl;

  ui->progressBar->setValue(0);
  setFrameNumber(0);
  frameNumber_aux = 0;
  timer->start(33);

  // disable options
}

void MainWindow::stopTimer()
{
  if (!timer->isActive())
    return;

  std::cout << "stopTimer()" << std::endl;

  timer->stop();
  //setFrameNumber(0);
  //ui->progressBar->setValue(0);

  destroyBGS();

  if (useCamera || useVideo)
    capture.release();

  // enable options
}

void MainWindow::setFrameNumber(long long _frameNumber)
{
  //std::cout << "setFrameNumber()" << std::endl;
  frameNumber = _frameNumber;
  QString txt_frameNumber = QString::fromStdString(its(frameNumber));
  ui->label_framenumber_txt->setText(txt_frameNumber);
}

bool MainWindow::setUpCapture()
{
  capture_length = 0;

  if (useCamera && !setUpCamera()) {
    std::cout << "Cannot initialize webcamera!" << std::endl;
    return false;
  }

  if (useVideo && !setUpVideo()) {
    std::cout << "Cannot open video file " << fileName.toStdString() << std::endl;
    return false;
  }

  if (useSequence && !setUpSequence()) {
    std::cout << "Cannot process images at " << fileName.toStdString() << std::endl;
    return false;
  }

  if (useCamera || useVideo) {
    int capture_fps = capture.get(CV_CAP_PROP_FPS);
    std::cout << "capture_fps: " << capture_fps << std::endl;
  }

  if (useVideo) {
    capture_length = capture.get(CV_CAP_PROP_FRAME_COUNT);
    std::cout << "capture_length: " << capture_length << std::endl;
  }

  std::cout << "OK!" << std::endl;
  return true;
}

void MainWindow::startCapture()
{
  //std::cout << "startCapture()" << std::endl;
  setFrameNumber(frameNumber + 1);
  cv::Mat cv_frame;

  if (useCamera || useVideo)
    capture >> cv_frame;

  if (useSequence && (frameNumber - 1) < entryList.length())
  {
    QString file = entryList.at(frameNumber - 1);
    QString filePath = QDir(fileName).filePath(file);

    std::cout << "Processing: " << filePath.toStdString() << std::endl;
    if (fileExists(filePath))
      cv_frame = cv::imread(filePath.toStdString());
  }

  if (cv_frame.empty())
  {
    stopTimer();
    return;
  }

  if (frameNumber == 1)
  {
    int frame_width = cv_frame.size().width;
    int frame_height = cv_frame.size().height;
    ui->label_frameresw_txt->setText(QString::fromStdString(its(frame_width)));
    ui->label_frameresh_txt->setText(QString::fromStdString(its(frame_height)));
  }

  if (useVideo && capture_length > 0)
  {
    double perc = (double(frameNumber) / double(capture_length)) * 100.0;
    //std::cout << "perc: " << perc << std::endl;
    ui->progressBar->setValue(perc);
  }

  int startAt = ui->spinBox_startat->value();
  if (startAt > 0 && frameNumber < startAt)
  {
    timer->setInterval(1);
    return;
  }
  else
    timer->setInterval(33);

  int stopAt = ui->spinBox_stopat->value();
  if (stopAt > 0 && frameNumber >= stopAt)
  {
    stopTimer();
    return;
  }

  cv::Mat cv_frame_small;
  cv::resize(cv_frame, cv_frame_small, cv::Size(250, 250));

  QImage qt_frame = cv2qimage(cv_frame_small);
  ui->label_img_in->setPixmap(QPixmap::fromImage(qt_frame, Qt::MonoOnly));

  processFrame(cv_frame);
}

QImage MainWindow::cv2qimage(const cv::Mat &cv_frame)
{
  if (cv_frame.channels() == 3)
    return Mat2QImage(cv_frame);
  else
    return GrayMat2QImage(cv_frame);
}

void MainWindow::processFrame(const cv::Mat &cv_frame)
{
  cv::Mat cv_fg;
  cv::Mat cv_bg;
  tic();
  bgs->process(cv_frame, cv_fg, cv_bg);
  toc();
  ui->label_fps_txt->setText(QString::fromStdString(its(fps())));

  cv::Mat cv_fg_small;
  cv::resize(cv_fg, cv_fg_small, cv::Size(250, 250));
/*添加遗留物判定*/
  Mat framec = cv_frame.clone();
 // cv::resize(framec, framec, cv::Size(250, 250));
  std::vector<std::vector<cv::Point>> contours;
  incomeroi oneroi;
  if(cv_fg.channels()>1)
  {
  cvtColor(cv_fg, cv_fg, COLOR_BGR2GRAY);
  }
  cv::Mat element = getStructuringElement(MORPH_RECT, cv::Size(5, 5));
  cv::erode(cv_fg, cv_fg, element);
  cv::findContours(cv_fg, contours, RETR_LIST, CHAIN_APPROX_NONE);
 // drawContours(framec,contours,-1,Scalar(255,255,255),4,8);
  vector<incomeroi> incomerois;
  for (int i = 0; i < contours.size(); i++)
  {
      oneroi.area = contourArea(contours[i]);
      if (oneroi.area < 50) continue;/*小于50的物体，直接看作点，排除掉*/
      Moments mu;
      mu = moments(contours[i], false);
      //计算轮廓的质心
      oneroi.centroid = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
      minAreaRect(contours[i]);
      Rect rect = boundingRect(contours[i]);
      oneroi.box = rect;
      incomerois.push_back(oneroi);
  }


  vector<Rect> rects;
  ObjTracker(incomerois, rects);

  for (int recti = 0; recti < rects.size(); recti++)
  {
      Point p1 = Point(rects[recti].tl().x, rects[recti].tl().y);
      Point p2 = Point(rects[recti].br().x, rects[recti].br().y);
      rectangle(framec, p1, p2, Scalar(255, 0, 0), 3, 8);
  }
 // imshow(" ",framec);
 // waitKey(1);
  cv::resize(framec, framec, cv::Size(250, 250));
  QImage qt_fg = cv2qimage(framec);
  ui->label_img_fg->setPixmap(QPixmap::fromImage(qt_fg, Qt::MonoOnly));
/*判定结束*/

 // QImage qt_fg = cv2qimage(cv_fg_small);
 // ui->label_img_fg->setPixmap(QPixmap::fromImage(qt_fg, Qt::MonoOnly));

  //cv::Mat cv_bg_small;
  //cv::resize(cv_bg, cv_bg_small, cv::Size(250, 250));
  //QImage qt_bg = cv2qimage(cv_bg_small);
 // ui->label_img_bg->setPixmap(QPixmap::fromImage(qt_bg, Qt::MonoOnly));

  if (ui->checkBox_save_im->isChecked() || ui->checkBox_save_fg->isChecked() || ui->checkBox_save_bg->isChecked())
  {
    if (ui->checkBox_kfn->isChecked())
      frameNumber_aux = frameNumber;
    else
      frameNumber_aux = frameNumber_aux + 1;
  }
  if (ui->checkBox_save_im->isChecked())
  {
    QString out_im_path = ui->lineEdit_out_in->text();
    QString out_im_file = QDir(out_im_path).filePath(QString::number(frameNumber_aux) + ".png");
    cv::imwrite(out_im_file.toStdString(), cv_frame);
  }
  if (ui->checkBox_save_fg->isChecked())
  {
    QString out_im_path = ui->lineEdit_out_fg->text();
    QString out_im_file = QDir(out_im_path).filePath(QString::number(frameNumber_aux) + ".png");
    cv::imwrite(out_im_file.toStdString(), cv_fg);
  }
  if (ui->checkBox_save_bg->isChecked())
  {
    QString out_im_path = ui->lineEdit_out_bg->text();
    QString out_im_file = QDir(out_im_path).filePath(QString::number(frameNumber) + ".png");
    cv::imwrite(out_im_file.toStdString(), cv_bg);
  }
}

void MainWindow::tic()
{
  duration = static_cast<double>(cv::getTickCount());
}

void MainWindow::toc()
{
  duration = (static_cast<double>(cv::getTickCount()) - duration) / cv::getTickFrequency();
  //std::cout << "time(sec):" << std::fixed << std::setprecision(6) << duration << std::endl;
  //std::cout << duration << std::endl;
}

double MainWindow::fps()
{
  //double fps  = frameNumber / duration;
  double fps = 1 / duration;
  //std::cout << "Estimated frames per second : " << fps << std::endl;
  return fps;
}

bool MainWindow::setUpCamera()
{
  int cameraIndex = ui->spinBox_webcamera->value();
  std::cout << "Camera index: " << cameraIndex << std::endl;

  capture.open(cameraIndex);
  return capture.isOpened();
}

bool MainWindow::setUpVideo()
{
  std::string videoFileName = fileName.toStdString();
  std::cout << "Openning: " << videoFileName << std::endl;
  capture.open(videoFileName.c_str());
  return capture.isOpened();
}

bool MainWindow::setUpSequence()
{
  std::cout << "Directory path: " << fileName.toStdString() << std::endl;
  if (QDir(fileName).exists())
  {
    QDir dir(fileName);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp" << "*.gif";
    dir.setNameFilters(filters);
    //entryList = dir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    //entryList = dir.entryList(QDir::Filter::Files, QDir::SortFlag::NoSort);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::NoSort);  // will sort manually with std::sort
    //dir.setSorting(QDir::LocaleAware);
    entryList = dir.entryList();

    std::cout << entryList.length() << std::endl;
    if (entryList.length() == 0)
    {
      QMessageBox::warning(this, "Warning", "No image found! (png, jpg, bmp, gif)");
      return false;
    }

    QCollator collator;
    collator.setNumericMode(true);
    std::sort(
      entryList.begin(),
      entryList.end(),
      [&collator](const QString &file1, const QString &file2)
    {
      return collator.compare(file1, file2) < 0;
    });

    //    for(int i = 0; i < entryList.length(); i++)
    //    {
    //      QString file = entryList.at(i);
    //      std::cout << file.toStdString() << std::endl;
    //    }
    return true;
  }
  else
  {
    QMessageBox::warning(this, "Warning", "Directory path doesn't exist!");
    return false;
  }
}

QString MainWindow::getSelectedAlgorithmName()
{
  QModelIndex index = ui->listView_algorithms->currentIndex();
  QString algorithm_name = index.data(Qt::DisplayRole).toString();
  return algorithm_name;
}

void MainWindow::on_listView_algorithms_doubleClicked(const QModelIndex &index)
{
  QString algorithm_name = index.data(Qt::DisplayRole).toString();
  std::cout << "Selected algorithm: " << algorithm_name.toStdString() << std::endl;

  //  CodeEditor editor;
  //  editor.setWindowTitle(QObject::tr("Code Editor Example"));
  //  editor.show();

  QString configFileName = QDir(".").filePath("config/" + algorithm_name + ".xml");
  std::cout << "Looking for: " << configFileName.toStdString() << std::endl;

  if (fileExists(configFileName))
  {
    textEditor.loadFile(configFileName);
    textEditor.show();
  }
  else
  {
    QMessageBox::warning(this, "Warning", "XML configuration file not found!\nPlease run the algorithm first!");
    return;
  }
}
