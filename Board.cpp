#include "Board.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
//取行和列
#define GetRowCol(__row, __col, __id) \
    int __row = _s[__id]._row;\
    int __col = _s[__id]._col
//构造函数
Board::Board(QWidget *parent) : QFrame(parent)
{
    this->_r = 20; //棋子半径长度为20
    setMinimumSize(_r*18+1, _r*20+1);
    init(true); //初始化
}

Board::~Board()
{

}
//初始化棋子
void Board::init(bool bRedSide)
{
    for(int i=0; i<32; ++i)
    {
        _s[i].init(i); //每个都调用初始化函数
    }
    if(bRedSide)
    {
        for(int i=0; i<32; ++i)
        {
            _s[i].rotate();
        }
    }

    _selectid = -1; //被选中的棋子id，默认为-1
    _bRedTurn = true; //一开始是红先走
    _bSide = bRedSide;
    update();
}
//画棋盘所有东西
void Board::paintEvent(QPaintEvent *)
{
    int r = height()/20;
    _r = r;
    _off = QPoint(r+1, r+1);

    QPainter p(this); //定义painter对象

    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    p.save();
    drawPlate(p); //绘制棋盘线
    p.restore();

    p.save();
    drawPlace(p);
    p.restore();

    p.save();
    drawInitPosition(p);
    p.restore();

    p.save();
    drawStone(p);//绘制棋子
    p.restore();
}
//绘制棋子1
void Board::drawStone(QPainter &p)
{
    for(int i=0; i<32; i++)
    {
        drawStone(p, i);
    }
}
//判断是否已被杀
bool Board::isDead(int id)
{
    if(id == -1)return true; //被杀返回true
    return _s[id]._dead;
}
//绘制棋子2，参数有绘画对象（引用，因为要改变这个对象的值，不能用引用）以及棋子对应的id
void Board::drawStone(QPainter &p, int id)
{
    if(isDead(id)) return; //如果被杀就不绘制了

    //设置颜色
    QColor color;
    if(red(id)) color = Qt::red; //红棋红色
    else color = Qt::black; //黑棋黑色

    p.setPen(QPen(QBrush(color), 2));

    //设置填充颜色
    if(id == _selectid) p.setBrush(Qt::gray);//被选中则灰色
    else p.setBrush(Qt::yellow); //否则是黄色

    p.drawEllipse(cell(id));//圆心就是该id的行和列

    //设置字体
    p.setFont(QFont("system", _r*1.2, 700));//设置大小和粗度
    p.drawText(cell(id), name(id), QTextOption(Qt::AlignCenter)); //name是stone里的名字函数，第三个参数对齐方式是中间对齐u
}


QString Board::name(int id)
{
    return _s[id].name();
}
//判断此棋子是否是红方棋子
bool Board::red(int id)
{
    return _s[id]._red;
}

void Board::drawInitPosition(QPainter &p, int row, int col)
{
    QPoint pt = center(row, col);
    QPoint off = QPoint(_r/6, _r/6);
    int len = _r/3;

    QPoint ptStart;
    QPoint ptEnd;

    if(col != 0)
    {
        /* 左上角 */
        ptStart = QPoint(pt.x() - off.x(), pt.y() - off.y());
        ptEnd = ptStart + QPoint(-len, 0);
        p.drawLine(ptStart, ptEnd);
        ptEnd = ptStart + QPoint(0, -len);
        p.drawLine(ptStart, ptEnd);

        /* 左下角 */
        ptStart = QPoint(pt.x() - off.x(), pt.y() + off.y());
        ptEnd = ptStart + QPoint(-len, 0);
        p.drawLine(ptStart, ptEnd);
        ptEnd = ptStart + QPoint(0, +len);
        p.drawLine(ptStart, ptEnd);
    }

    if(col != 8)
    {
        /* 右下角 */
        ptStart = QPoint(pt.x() + off.x(), pt.y() + off.y());
        ptEnd = ptStart + QPoint(+len, 0);
        p.drawLine(ptStart, ptEnd);
        ptEnd = ptStart + QPoint(0, +len);
        p.drawLine(ptStart, ptEnd);

        /* 右上角 */
        ptStart = QPoint(pt.x() + off.x(), pt.y() - off.y());
        ptEnd = ptStart + QPoint(+len, 0);
        p.drawLine(ptStart, ptEnd);
        ptEnd = ptStart + QPoint(0, -len);
        p.drawLine(ptStart, ptEnd);
    }
}

void Board::drawInitPosition(QPainter &p)
{
    drawInitPosition(p, 3, 0);
    drawInitPosition(p, 3, 2);
    drawInitPosition(p, 3, 4);
    drawInitPosition(p, 3, 6);
    drawInitPosition(p, 3, 8);

    drawInitPosition(p, 6, 0);
    drawInitPosition(p, 6, 2);
    drawInitPosition(p, 6, 4);
    drawInitPosition(p, 6, 6);
    drawInitPosition(p, 6, 8);

    drawInitPosition(p, 2, 1);
    drawInitPosition(p, 2, 7);

    drawInitPosition(p, 7, 1);
    drawInitPosition(p, 7, 7);
}

void Board::drawPlace(QPainter &p)
{
    p.drawLine(center(0, 3), center(2, 5));
    p.drawLine(center(2, 3), center(0, 5));

    p.drawLine(center(9, 3), center(7, 5));
    p.drawLine(center(7, 3), center(9, 5));
}

void Board::drawPlate(QPainter &p) //画棋盘线，横10线，竖9线，九宫格
{
    for(int i=0; i<10; ++i) //横10条线
    {
        if(i==0 || i==9)
        {
            p.setPen(QPen(Qt::black, 3, Qt::SolidLine));
        }
        else
        {
            p.setPen(QPen(Qt::black, 1, Qt::SolidLine));
        }
        p.drawLine(center(i, 0), center(i, 8));
    }

    for(int i=0; i<9; ++i) //竖着9条线
    {
        if(i==0 || i==8)
        {
            p.setPen(QPen(Qt::black, 3, Qt::SolidLine));
            p.drawLine(center(0, i), center(9, i));
        }
        else
        {
            p.setPen(QPen(Qt::black, 1, Qt::SolidLine));
            p.drawLine(center(0, i), center(4, i));
            p.drawLine(center(5, i), center(9, i));
        }
    }
}

QPoint Board::center(int row, int col) //将输入的行和列转换成其圆心的位置
{
    QPoint pt(_r*col*2, _r*row*2); //因为是半径所以要乘2
    return pt + _off;
}
//重要center函数，函数重载
QPoint Board::center(int id) //根据输入的棋子的id返回相应的行和列
{
    return center(_s[id]._row, _s[id]._col); //返回的是上面重载函数的返回值即圆心
}

QPoint Board::topLeft(int row, int col)
{
    return center(row, col) - QPoint(_r, _r);
}

QPoint Board::topLeft(int id)
{
    return center(id) - QPoint(_r, _r);
}

QRect Board::cell(int row, int col)
{
    return QRect(topLeft(row, col), QSize(_r*2-1, _r*2-1));
}

QRect Board::cell(int id)
{
    return QRect(topLeft(id), QSize(_r*2-1, _r*2-1));
}
//判断是否点击到了某个棋子的附近（虽然返回的是布尔类型，但是row和col是引用，如果成功则row和col也随之改变）
bool Board::getClickRowCol(QPoint pt, int &row, int &col)
{
    for(row=0; row<=9; ++row)
    {
        for(col=0; col<=8; ++col)
        {
            QPoint distance = center(row, col) - pt;
            if(distance.x() * distance.x() + distance.y() * distance.y() < _r* _r)
                return true;
        }
    }
    return false;
}
//走棋函数(斜线表示虚函数)
void Board::mouseReleaseEvent(QMouseEvent *ev) //鼠标释放时动作，ev是所点位置
{
    if(ev->button() != Qt::LeftButton)
    {
        return;
    }

    click(ev->pos());
}
//验证是否是能走的阵营
bool Board::canSelect( int id)
{
    return _bRedTurn == _s[id]._red;
}
//验证选择的棋子，包括是否被杀，是否是能走的阵营
void Board::trySelectStone(int id)
{
    if(id == -1)
        return;

    if(!canSelect(id)) return;
    //没有问题则把id传入
    _selectid = id;
    update();//重绘棋盘
}
//验证选中的棋子和即将被杀的棋子是否是同一阵营
bool Board::sameColor(int id1, int id2)
{
    if(id1 == -1 || id2 == -1) return false; //如果有棋子已经被杀了则返回

    return red(id1) == red(id2); //两个棋子是属于同一阵营
}
//移动棋子
void Board::tryMoveStone(int killid, int row, int col)
{
    if(killid != -1 && sameColor(killid, _selectid))
    {
        trySelectStone(killid);
        return;
    }

    bool ret = canMove(_selectid, killid, row, col);
    if(ret)
    {
        moveStone(_selectid, killid, row, col);
        _selectid = -1;
        update();
    }
}
//获取两个棋子之间的棋子个数
int Board::getStoneCountAtLine(int row1, int col1, int row2, int col2)
{
    int ret = 0;
    if(row1 != row2 && col1 != col2) //不在一条直线上
        return -1;
    if(row1 == row2 && col1 == col2) //在一个点上
        return -1;

    if(row1 == row2)
    {
        int min = col1 < col2 ? col1 : col2;
        int max = col1 < col2 ? col2 : col1;
        for(int col = min+1; col<max; ++col)
        {
            if(getStoneId(row1, col) != -1) ++ret;
        }
    }
    else
    {
        int min = row1 < row2 ? row1 : row2;
        int max = row1 < row2 ? row2 : row1;
        for(int row = min+1; row<max; ++row)
        {
            if(getStoneId(row, col1) != -1) ++ret;
        }
    }

    return ret;
}

// 计算两个棋子之间的数学关系，用来
int Board::relation(int row1, int col1, int row, int col)
{
    return qAbs(row1-row)*10+qAbs(col1-col);
}
//车移动规则
bool Board::canMoveChe(int moveid, int, int row, int col)
{
    GetRowCol(row1, col1, moveid);
    int ret = getStoneCountAtLine(row1, col1, row, col);
    if(ret == 0)
        return true;
    return false;
}
//马移动规则
bool Board::canMoveMa(int moveid, int, int row, int col)
{
    GetRowCol(row1, col1, moveid);
    int r = relation(row1, col1, row, col);
    if(r != 12 && r != 21)
        return false;

    if(r == 12)
    {
        if(getStoneId(row1, (col+col1)/2) != -1)
            return false;
    }
    else
    {
        if(getStoneId((row+row1)/2, col1) != -1)
            return false;
    }

    return true;
}

bool Board::canMovePao(int moveid, int killid, int row, int col)
{
    GetRowCol(row1, col1, moveid);
    int ret = getStoneCountAtLine(row, col, row1, col1);
    if(killid != -1)
    {
        if(ret == 1) return true;
    }
    else
    {
        if(ret == 0) return true;
    }
    return false;
}

bool Board::canMoveBing(int moveid, int, int row, int col)
{
    GetRowCol(row1, col1, moveid);
    int r = relation(row1, col1, row, col);
    if(r != 1 && r != 10) return false;

    //上面下面
    if(isBottomSide(moveid))//在上面智能往下走
    {
        if(row > row1) return false;
        if(row1 >= 5 && row == row1) return false; //没过河不能横着走
    }
    else //下面只能往上走
    {
        if(row < row1) return false;
        if(row1 <= 4 && row == row1) return false;
    }

    return true;
}
//将移动规则
bool Board::canMoveJiang(int moveid, int killid, int row, int col)
{
    if(killid != -1 && _s[killid]._type == Stone::JIANG)
        return canMoveChe(moveid, killid, row, col);

    //两个将不能相遇
    GetRowCol(row1, col1, moveid);
    int r = relation(row1, col1, row, col);
    if(r != 1 && r != 10) return false;

    //目标在九宫格内
    if(col < 3 || col > 5) return false;//列不在3-5内直接排除

    if(isBottomSide(moveid)) //先判断阵营
    {
        if(row < 7) return false; //黑棋的将不能小于7
    }
    else
    {
        if(row > 2) return false;  //红色的将不能大于2
    }
    return true;
}

bool Board::canMoveShi(int moveid, int, int row, int col)
{
    GetRowCol(row1, col1, moveid);
    int r = relation(row1, col1, row, col);
    if(r != 11) return false;

    if(col < 3 || col > 5) return false;
    if(isBottomSide(moveid))
    {
        if(row < 7) return false;
    }
    else
    {
        if(row > 2) return false;
    }
    return true;
}
//相移动规则
bool Board::canMoveXiang(int moveid, int, int row, int col) //第二个参数不用，成为哑元
{
    GetRowCol(row1, col1, moveid);

    //获取关系值
    int r = relation(row1, col1, row, col);
    if(r != 22) return false;

    //判断相眼上是否有棋子
    int rEye = (row+row1)/2;
    int cEye = (col+col1)/2;
    if(getStoneId(rEye, cEye) != -1)
        return false;

    //判断是在下方还是上方
    if(isBottomSide(moveid))
    {
        if(row < 4) return false;
    }
    else
    {
        if(row > 5) return false;
    }
    return true;
}
//判断是否能走，存在走棋和吃棋两种情况
bool Board::canMove(int moveid, int killid, int row, int col)//参数：移动的，移动到的，行，列
{
    if(sameColor(moveid, killid)) return false; //如果移动的和被移动到的是一方则不能走

    switch (_s[moveid]._type) //多种情况使用switch和case
    {
    case Stone::CHE:
        return canMoveChe(moveid, killid, row, col);

    case Stone::MA:
        return canMoveMa(moveid, killid, row, col);

    case Stone::PAO:
        return canMovePao(moveid, killid, row, col);

    case Stone::BING:
        return canMoveBing(moveid, killid, row, col);

    case Stone::JIANG:
        return canMoveJiang(moveid, killid, row, col);

    case Stone::SHI:
        return canMoveShi(moveid, killid, row, col);

    case Stone::XIANG:
        return canMoveXiang(moveid, killid, row, col);

    }
    return false;
}


void Board::reliveStone(int id)
{
    if(id==-1) return;
    _s[id]._dead = false;
}

void Board::killStone(int id)
{
    if(id==-1) return;
    _s[id]._dead = true;
}
//判断是在下方还是在上方（可以用哪边阵营来判断）
bool Board::isBottomSide(int id)
{
    return _bSide == _s[id]._red;
}

void Board::moveStone(int moveid, int row, int col)
{
    _s[moveid]._row = row;
    _s[moveid]._col = col;

    _bRedTurn = !_bRedTurn;
}
//移动信息
void Board::saveStep(int moveid, int killid, int row, int col, QVector<Step*>& steps)
{
    GetRowCol(row1, col1, moveid);
    Step* step = new Step;//生成新的对象
    step->_colFrom = col1;
    step->_colTo = col;
    step->_rowFrom = row1;
    step->_rowTo = row;
    step->_moveid = moveid;
    step->_killid = killid;

    steps.append(step); //将对象放入数组
}

void Board::moveStone(int moveid, int killid, int row, int col)
{
    saveStep(moveid, killid, row, col, _steps);

    killStone(killid);
    moveStone(moveid, row, col);
}
//点击棋子(虚函数，子类重写)
void Board::click(int id, int row, int col)
{
    if(this->_selectid == -1)//默认状态，表示未选中棋子
    {
        trySelectStone(id); //将id验证后传给 _selectid
    }
    else //已经选中了棋子，开始移动的环节
    {
        tryMoveStone(id, row, col);
    }
}
//根据点击到的像素返回点击到的棋子，行和列
void Board::click(QPoint pt)
{
    int row, col;
    bool bClicked = getClickRowCol(pt, row, col); //判断是否满足要求，如果满足要求row和col现在就是满足要求的值
    if(!bClicked) return;

    int id = getStoneId(row, col);
    click(id, row, col);
}
//根据选中的位置得到该位置棋子id
int Board::getStoneId(int row, int col)
{
    for(int i=0; i<32; ++i)
    {
        if(_s[i]._row == row && _s[i]._col == col && !isDead(i)) //点击到的位置有棋子
            return i;
    }
    return -1;
}

void Board::back(Step *step)
{
    reliveStone(step->_killid);
    moveStone(step->_moveid, step->_rowFrom, step->_colFrom);
}

void Board::backOne()
{
    if(this->_steps.size() == 0) return;

    Step* step = this->_steps.last();
    _steps.removeLast();
    back(step);

    update();
    delete step;
}

void Board::back()
{
    backOne();
}

void Board::slotBack()
{
    back();
}
