#include "playscene.h"
#include<QMenuBar>
#include<QPainter>
#include"mypushbutton.h"
#include<QTimer>
#include<QLabel>
#include"mycoin.h"
#include"dataconfig.h"
#include<QPropertyAnimation>
#include<QSoundEffect>

// PlayScene::PlayScene(QWidget *parent)
//     : QMainWindow{parent}
// {}

PlayScene::PlayScene(int levelNum)
{
    QString str = QString("进入了第 %1 关").arg(levelNum);
    qDebug()<<str;
    this->levelIndex = levelNum;

    //初始化游戏场景
    //设置固定大小
    setFixedSize(320,588);
    //设置图标
    setWindowIcon(QIcon(":/res/Coin0001.png"));
    //设置标题
    setWindowTitle("翻金币场景");

    //创建菜单栏
    QMenuBar * bar = menuBar();
    setMenuBar(bar);

    //创建开始菜单
    QMenu * startMenu = bar->addMenu("开始");

    //创建退出 菜单项
    QAction * quitAction = startMenu->addAction("退出");

    //点击退出 实现退出游戏
    connect(quitAction,&QAction::triggered,[=](){
        this->close();
    });

    //添加音效资源
    //返回按钮音效
    QSoundEffect *backSound = new QSoundEffect;
    backSound->setSource(QUrl::fromLocalFile(":/res/BackButtonSound.wav"));
    //翻金币音效
    QSoundEffect *flipSound = new QSoundEffect;
    flipSound->setSource(QUrl::fromLocalFile(":/res/ConFlipSound.wav"));
    //胜利音效
    QSoundEffect *winSound = new QSoundEffect;
    winSound->setSource(QUrl::fromLocalFile(":/res/LevelWinSound.wav"));

    //返回按钮
    MyPushButton * backBtn = new MyPushButton(":/res/BackButton.png",":/res/BackButtonSelected.png");
    backBtn->setParent(this);
    backBtn->move(this->width() - backBtn->width(),this->height() - backBtn->height());

    //点击返回
    connect(backBtn,&MyPushButton::clicked,[=](){
        //qDebug()<<"翻金币场景中：点击了返回按钮";
        //播放返回按钮音效
        backSound->play();

        QTimer::singleShot(500,this,[=](){
            emit this->chooseSceneBack();
        });

    });


    //显示当前关卡数
    QLabel * label = new QLabel;
    label->setParent(this);
    QFont font;
    font.setFamily("华文新魏");
    font.setPointSize(20);
    QString str1 = QString("Level: %1").arg(this->levelIndex);
    //将字体设置到标签控件中
    label->setFont(font);
    label->setText(str1);
    label->setGeometry(30,this->height() - 50,120,50);


    dataConfig config;
    //初始化每个关卡的二维数组
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            this->gameArray[j][i] = config.mData[this->levelIndex][i][j];

        }

    }


    //胜利图片显示
    QLabel* winLabel = new QLabel;
    QPixmap tmpPix;
    tmpPix.load(":/res/LevelCompletedDialogBg.png");
    winLabel->setGeometry(0,0,tmpPix.width(),tmpPix.height());
    winLabel->setPixmap(tmpPix);
    winLabel->setParent(this);
    winLabel->move((this->width() - tmpPix.width())*0.5,-tmpPix.height());


    //显示金币背景的图案
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //绘制背景图案
            QPixmap pix = (QPixmap(":/res/BoardNode.png"));
            QLabel* label = new QLabel;
            label->setGeometry(0,0,pix.width(),pix.height());
            label->setPixmap(pix);
            label->setParent(this);
            label->move(57 + i * 50,200 + j * 50);


            //创建金币
            QString str;
            if(this->gameArray[i][j] == 1){
                //显示金币
                str = ":/res/Coin0001.png";
            }
            else
            {
                str = ":/res/Coin0008.png";
            }
            MyCoin * coin = new MyCoin(str);
            coin->setParent(this);
            coin->move(59 + i * 50,204 + j * 50);

            //给金币属性赋值
            coin->posX = i;
            coin->posY = j;
            coin->flag = this->gameArray[j][i];//1正面 0反面

            //将金币放入到 金币的二维数组里 以便以后的维护
            coinBtn[i][j] = coin;

            //点击金币 进行翻转
            connect(coin,&MyCoin::clicked,[=](){
                //播放翻金币音效
                flipSound->play();

                coin->changeFlag();
                this->gameArray[j][i] = this->gameArray[j][i] == 0 ? 1 : 0;

                //翻转周围的金币
                //周围的右侧金币翻转条件
                if(coin->posX + 1 <= 3)
                {
                    coinBtn[coin->posX+1][coin->posY]->changeFlag();
                    this->gameArray[coin->posX+1][coin->posY] = this->gameArray[coin->posX+1][coin->posY] == 0 ? 1 : 0;
                }
                //周围的左侧金币翻转条件
                if(coin->posX - 1 >= 0)
                {
                    coinBtn[coin->posX-1][coin->posY]->changeFlag();
                    this->gameArray[coin->posX-1][coin->posY] = this->gameArray[coin->posX-1][coin->posY] == 0 ? 1 : 0;
                }
                //周围的下侧金币翻转条件
                if(coin->posY + 1 <= 3)
                {
                    coinBtn[coin->posX][coin->posY+1]->changeFlag();
                    this->gameArray[coin->posX][coin->posY+1] = this->gameArray[coin->posX][coin->posY+1] == 0 ? 1 : 0;
                }
                //周围的上侧金币翻转条件
                if(coin->posY - 1 >= 0)
                {
                    coinBtn[coin->posX][coin->posY-1]->changeFlag();
                    this->gameArray[coin->posX][coin->posY-1] = this->gameArray[coin->posX][coin->posY-1] == 0 ? 1 : 0;
                }

                //判断是否胜利
                this->isWin = true;
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        if(coinBtn[i][j]->flag == false)//只要有一个是反面，那就算失败
                        {
                            this->isWin = false;
                            break;
                        }

                    }

                }

                if(this->isWin == true)
                {
                    //播放胜利音效
                    winSound->play();
                    //胜利了！
                    qDebug()<<"游戏胜利！";
                    //将所有按钮的胜利标志改为true；如果再次点击按钮，直接return，不做响应
                    for (int i = 0; i < 4; i++) {
                        for (int j = 0; j < 4; j++) {
                            coinBtn[i][j]->isWin =true;

                        }

                    }
                    //将胜利的图片移动下来
                    QPropertyAnimation * animation = new QPropertyAnimation(winLabel,"geometry");
                    //设置时间间隔
                    animation->setDuration(1000);
                    //设置开始位置
                    animation->setStartValue(QRect(winLabel->x(),winLabel->y(),winLabel->width(),winLabel->height()));
                    //设置结束位置
                    animation->setEndValue(QRect(winLabel->x(),winLabel->y()+114,winLabel->width(),winLabel->height()));
                    //设置缓和曲线
                    animation->setEasingCurve(QEasingCurve::OutBounce);
                    //执行动画
                    animation->start();
                }
            });
        }

    }
}


void PlayScene::paintEvent(QPaintEvent *)
{
    //创建背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/PlayLevelSceneBg.png");
    painter.drawPixmap(0,25,this->width(),this->height(),pix);

    //加载标题
    pix.load(":/res/Title.png");
    pix = pix.scaled(pix.width() * 0.5,pix.height() * 0.5);
    painter.drawPixmap(10,30,pix.width(),pix.height(),pix);
}
