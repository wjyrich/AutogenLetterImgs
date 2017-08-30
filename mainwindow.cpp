#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->minFontSizeBox->setValue(8);
    srand(time(0));
    connect(ui->generateBtn,SIGNAL(clicked(bool)),this,SLOT(generateBtnClicked()));
    connect(ui->massGenerateBtn,SIGNAL(clicked(bool)),this,SLOT(massGenerateBtnClicked()));

    GraphicsSceneEx *scene=new GraphicsSceneEx();
    ui->graphicsView->setScene(scene);
    pixmapItem=new QGraphicsPixmapItem();
    scene->addItem(pixmapItem);

    QString desktopDir=QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    ui->massGenerateDirBox->setText(desktopDir);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage *MainWindow::genImg(QString &tagSource, QString filenameTemplate, bool massGen)
{
    bool mixBgColors=ui->mixBgColorsBox->isChecked();
    bool drawRects=ui->drawRectsBox->isChecked();
    bool useBw=ui->bwBox->isChecked();
    bool mixFontColors=ui->mixFontColorsBox->isChecked();
    bool mixLineColors=ui->mixLineColorsBox->isChecked();
    bool arrangeLetters=ui->arrangeLettersBox->isChecked();
    bool invert=ui->invertBox->isChecked();
    bool addRandomLines=ui->addRandomLinesBox->isChecked();
    bool rotateLetters=ui->rotateLettersBox->isChecked();
    bool genFileForEachLetter=ui->genFileForEachLetterBox->isChecked() && massGen;
    bool genTrainValTxt=genFileForEachLetter && ui->genTrainTxtBox->isChecked();
    int rotateMinDegs=ui->rotateMinDegsBox->value();
    int rotateMaxDegs=ui->rotateMaxDegsBox->value();
    int rotateDegsVariety=(rotateMaxDegs-rotateMinDegs)+1;
    int randomLineCount=ui->randomLineCountBox->value();
    int gridSize=ui->gridSizeBox->value();
    int width=ui->widthBox->value();
    int height=ui->heightBox->value();
    int lettersPerLine=(int)floor((float)width/gridSize);
    int minFontSize=ui->minFontSizeBox->value();
    int fontSizeVariety=ui->maxFontSizeBox->value()-minFontSize+1;

    QImage *img=new QImage(width,height,QImage::Format_ARGB32);
    QImage *blankImg=new QImage(width,height,QImage::Format_ARGB32);
    QPainter p(img);
    QPainter blankP(blankImg);

    blankP.fillRect(0,0,width,height,QColor(255,255,255));

    int rC;
    int gC;
    int bC;

    if(mixBgColors&&addRandomLines)
    {
        rC=rand()%255;
        if(useBw)
        {
            while(rC<50)
                rC=rand()%255;
            gC=rC;
            bC=rC;
        }
        else
        {
            gC=rand()%255;
            bC=rand()%255;
        }

        QColor bgColor=QColor(rC,gC,bC);
        p.fillRect(0,0,width,height,bgColor);
        // Fill with random areas
        int n=50+rand()%50;
        for(int i=0;i<n;i++)
        {
            rC=rand()%255;
            if(useBw)
            {
                while(rC<50)
                    rC=rand()%255;
                gC=rC;
                bC=rC;
            }
            else
            {
                gC=rand()%255;
                bC=rand()%255;
            }
            QColor randCol=QColor(rC,gC,bC);
            int x=rand()%width;
            int y=rand()%height;
            int w=rand()%200;
            int h=rand()%200;
            p.fillRect(x,y,w,h,randCol);
        }
    }
    else
        p.fillRect(0,0,width,height,QBrush(QColor(invert?0:255,invert?0:255,invert?0:255)));


    const char *letters="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPYRSTUVWXYZ1234567890?=-.,:;<>(){}[]!\"'$%&/\\";
    int letterCount=strlen(letters);

    QStringList fontFamilies=ui->fontBox->text().replace(", ","").split(",");

    int fontFamilyCount=fontFamilies.count();


    // Add letters

    if(!genTrainValTxt)
        tagSource="{\n\
    \"fileFormatVersion\": 1.1,\n\
    \"minFileFormatVersion\": 1.1,\n\
    \"tagCount\": %tagcount%,\n\
    \"tags\":\n\
    [";

    int x;
    int y;

    QRect pictRect(0,0,width-(arrangeLetters?(gridSize/2):0),height-(arrangeLetters?(gridSize/2):0));
    QList<QRect> rects;
    QList<QRect> outRects;
    QList<int> letterNumList;
    QList<QString> letterStrList;
    if(!mixLineColors)
    {
        if(invert)
            p.setPen(QColor(255,255,255));
        else
            p.setPen(QColor(0,0,0));
    }
    QRect actualRect;
    int rectCount=0;
    int maxLetters=ui->maxLettersBox->value();
    int limitAttempts=5000;
    int rotateDegs;

    if(addRandomLines)
    {
        int x2;
        int y2;
        for(int i=0;i<randomLineCount;i++)
        {
            x=rand()%width;
            x2=rand()%width;
            y=rand()%height;
            y2=rand()%height;
            if(mixLineColors)
            {
                rC=rand()%255;
                if(useBw)
                {
                    if(!mixBgColors)
                    {
                        while(rC<50||rC>200)
                            rC=rand()%255;
                    }
                    gC=rC;
                    bC=rC;
                }
                else
                {
                    gC=rand()%255;
                    bC=rand()%255;
                }
                QColor fontCol;
                if(invert)
                    fontCol=QColor(255-rC,255-gC,255-bC);
                else
                    fontCol=QColor(rC,gC,bC);
                p.setPen(fontCol);
            }
            p.drawLine(x,y,x2,y2);
        }
    }

    QColor fontCol;

    if(!mixFontColors) // Do not join with "if(!mixLineColors)"
    {
        if(invert)
            p.setPen(fontCol=QColor(255,255,255));
        else
            p.setPen(fontCol=QColor(0,0,0));
    }

    for(int i=0;i<maxLetters;i++)
    {
        int letterNum=rand()%letterCount;
        char letter=letters[letterNum];
        QString str=QString::fromLatin1(letters+letterNum,1);
        int fSize=minFontSize+rand()%fontSizeVariety;
        QString family=fontFamilies.at(rand()%fontFamilyCount);
        QFont f(family,fSize,-1);
        p.setFont(f);
        QFontMetrics fm=p.fontMetrics();
        int fW=fm.width(letter);
        int fH=fm.height();
        QRect bigRect;
        int k=0;
        for(;k<limitAttempts;k++)
        {
            if(arrangeLetters)
            {
                x=(((rectCount+k)%lettersPerLine)+1)*gridSize;
                y=(((int)floor((float)((rectCount+k)/lettersPerLine)))+1)*gridSize;
            }
            else
            {
                x=rand()%width;
                y=rand()%height;
            }
            bigRect=QRect(x,y,fW,fH);
            bool cont=false;
            if(pictRect.contains(bigRect))
            {
                for(int j=0;j<rectCount;j++)
                {
                    if(rects.at(j).intersects(bigRect))
                    {
                        cont=true;
                        break;
                    }
                }
                if(cont)
                    continue;
                else
                    break;
            }
        }
        if(k==limitAttempts)
            break;
        if(mixFontColors)
        {
            rC=rand()%255;
            if(useBw)
            {
                if(!mixBgColors)
                {
                    while(rC<50||rC>200)
                        rC=rand()%255;
                }
                gC=rC;
                bC=rC;
            }
            else
            {
                gC=rand()%255;
                bC=rand()%255;
            }
            if(invert)
                fontCol=QColor(255-rC,255-gC,255-bC);
            else
                fontCol=QColor(rC,gC,bC);
            p.setPen(fontCol);
        }
        blankP.setFont(f);
        if(rotateLetters)
        {
            p.save();
            blankP.save();

            rotateDegs=rotateMinDegs+rand()%rotateDegsVariety;

            int xP=bigRect.x()+fW/2;
            int yP=bigRect.y()+fH/2;

            // QRect(bigRect.x()+fW/2,bigRect.y()+fH/2,fW*(rotateDegs>0?1:-1),fH*(rotateDegs>0?1:-1)).normalized()

            // height being considered too much!

            QTransform trf;
            /*0.9: balancing factor*/
            trf.translate(xP+sin(radians(rotateDegs))*fH*0.9f,yP-cos(radians(rotateDegs))*fH*0.9f);
            trf.rotate(rotateDegs);

            actualRect=trf.mapRect(fm.boundingRect(0,0,0,0,0,str));


            if(mixBgColors&&!addRandomLines)
            {
                QColor bgColor;
                for(;;)
                {
                    rC=rand()%255;
                    if(useBw)
                    {
                        while(rC<50)
                            rC=rand()%255;
                        gC=rC;
                        bC=rC;
                    }
                    else
                    {
                        gC=rand()%255;
                        bC=rand()%255;
                    }

                    bgColor=QColor(rC,gC,bC);
                    uint32_t color1=getColor(255,rC,bC,gC);
                    uint32_t color2=getColor(fontCol.alpha(),fontCol.red(),fontCol.green(),fontCol.blue());
                    double e=getColorError(color1,color2);
                    if(e<0.2f)
                        continue;
                    else
                        break;
                }
                p.fillRect(actualRect,bgColor);
            }

            p.translate(xP,yP);
            blankP.translate(xP,yP);

            p.rotate(rotateDegs);
            blankP.rotate(rotateDegs);

            p.drawText(0,0,str); // Do not use actualRect
            blankP.drawText(0,0,str);

            p.restore();
            blankP.restore();
        }
        else
        {
            blankP.drawText(bigRect,str);
            // Get actual rectangle without margin from blankImg
            int lowestX=width;
            int lowestY=height;
            int highestX=0;
            int highestY=0;
            for(int by=y;by<y+fH;by++)
            {
                for(int bx=x;bx<x+fW;bx++)
                {
                    if(blankImg->pixel(bx,by)==0xFF000000)
                    {
                        if(bx<lowestX)
                            lowestX=bx;
                        else if(bx>highestX)
                            highestX=bx;
                        if(by<lowestY)
                            lowestY=by;
                        else if(by>highestY)
                            highestY=by;
                    }
                }
            }
            actualRect=QRect(lowestX,lowestY,highestX-lowestX,highestY-lowestY);
            if(lowestX==width||lowestY==height||highestX==0||highestY==0)
                actualRect=bigRect;

            if(mixBgColors&&!addRandomLines)
            {
                QColor bgColor;
                for(;;)
                {
                    rC=rand()%255;
                    if(useBw)
                    {
                        while(rC<50)
                            rC=rand()%255;
                        gC=rC;
                        bC=rC;
                    }
                    else
                    {
                        gC=rand()%255;
                        bC=rand()%255;
                    }

                    bgColor=QColor(rC,gC,bC);
                    uint32_t color1=getColor(255,rC,bC,gC);
                    uint32_t color2=getColor(fontCol.alpha(),fontCol.red(),fontCol.green(),fontCol.blue());
                    double e=getColorError(color1,color2);
                    if(e<0.2f)
                        continue;
                    else
                        break;
                }
                p.fillRect(actualRect,bgColor);
            }
            p.drawText(bigRect,str); // Do not use actualRect
        }


        if(drawRects)
            p.drawRect(actualRect);
        rectCount++;
        rects.append(arrangeLetters?QRect(x,y,gridSize,gridSize):actualRect);
        if(genFileForEachLetter)
            outRects.append(actualRect);
        char aStr[2];
        aStr[0]=letter;
        aStr[1]=0;
        char *escapeStr=text::escapeDoubleQuotationMarks(aStr);
        QString escapeQStr=QString::fromLatin1(escapeStr);
        letterStrList.append(escapeQStr);
        letterNumList.append(letterNum);
        int aX=actualRect.x();
        int aY=actualRect.y();
        if(!genFileForEachLetter)
            tagSource+=QString(i>0?",":"")+QString("\n\
        [\"")+escapeQStr+QString("\", \"rect\", ")+QString::number(aX)+QString(", ")+QString::number(aY)+QString(", ")+QString::number(aX+actualRect.width())+QString(", ")+QString::number(aY+actualRect.height())+QString("]");
        free(escapeStr);
    }

    if(!genFileForEachLetter)
    {
        tagSource=tagSource.replace("%tagcount%",QString::number(rectCount))+"\n\
   ]\n\
}";
    }
    else
    {
        int rnd=rand();
        for(int j=0;j<rectCount;j++)
        {
            QRect r=outRects.at(j);
            QImage cpy=img->copy(r);
            QString filename=QString(filenameTemplate).replace("%rand%",QString::number(rnd)).replace("%number%",QString::number(j));
            cpy.save(filename,0,100);
            if(genTrainValTxt)
            {
                QFileInfo iF(filename);
                tagSource+=iF.fileName()+QString(" ")+QString::number(letterNumList.at(j))+QString("\n");
            }
            else
            {
                QFile f(filename+QString(".taglist.json"));
                f.open(QFile::WriteOnly|QFile::Truncate);
                QString tS=tagSource.replace("%tagcount%","1")+
                        "\n\
            [\""+letterStrList.at(j)+QString("\", \"rect\", 0, 0, ")+QString::number(r.width()-1)+QString(", ")+QString::number(r.height()-1)+"]\n\
        ]\n\
    }";
                f.write(tS.toUtf8());
                f.close();
            }
        }
    }
    blankP.end();
    delete blankImg;
    p.end();
    return img;
}

void MainWindow::generateBtnClicked()
{
    QString tagSource;
    QImage *img=genImg(tagSource);
    ui->tagBox->document()->setPlainText(tagSource);
    pixmapItem->setPixmap(QPixmap::fromImage(*img));
    ui->graphicsView->repaint();
}

void MainWindow::massGenerateBtnClicked()
{
    QString parentDir=ui->massGenerateDirBox->text().replace("\\","/");
    if(parentDir.endsWith("/"))
        parentDir=parentDir.left(parentDir.length()-1);
    QDir test(parentDir);
    if(!test.exists())
        parentDir=QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QDir d(parentDir);
    QString rd=QString::number(rand());
    d.mkdir(rd);
    QString dirPath=parentDir+QString("/")+rd;
    int amount=ui->massGenerateImgCountBox->value();
    bool genFileForEachLetter=ui->genFileForEachLetterBox->isChecked();
    QString tagSource=""; // Must be empty string!
    if(genFileForEachLetter)
    {
        for(int i=0;i<amount;i++)
        {
            QString rPath=dirPath+QString("/%rand%-%number%.jpg");
            QImage *img=genImg(tagSource,rPath,true);
            delete img;
        }
        if(ui->genTrainTxtBox->isChecked())
        {
            QByteArray arr=tagSource.toUtf8();

            QFile trainTxtFile(dirPath+QString("/train.txt"));
            trainTxtFile.open(QFile::WriteOnly|QFile::Truncate);
            trainTxtFile.write(arr);
            trainTxtFile.close();

            QFile valTxtFile(dirPath+QString("/val.txt"));
            valTxtFile.open(QFile::WriteOnly|QFile::Truncate);
            valTxtFile.write(arr);
            valTxtFile.close();
        }
    }
    else
    {
        for(int i=0;i<amount;i++)
        {
            QImage *img=genImg(tagSource);
            QString r=dirPath+QString("/")+QString::number(rand())+QString(".jpg");
            img->save(r,0,100);
            QFile f(r+QString(".taglist.json"));
            f.open(QFile::WriteOnly|QFile::Truncate);
            f.write(tagSource.toUtf8());
            f.close();
            delete img;
        }
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
}
