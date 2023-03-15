#include "Robot.h"
#include "Workbench.h"
#include <cstring>
#include "Params.h"
#define MAPLEN (100)

// 所有需要使用的全局变量
int Money;
string map[MAPLEN];
vector<Robot *> RobotVec;
vector<Workbench *> WorkBenchVec;

// 所有需要使用的函数
bool MapInit();
bool ReadInfo(int _WorkBenchNum, vector<Workbench *> _WorkBenchVec, vector<Robot *> _RobotVec);
double Buy(Robot *Robot, vector<Workbench *> &WBProductReady);
double MoveBuy(Robot *Robot);
int BuyFindVectorWBMinID(Robot *Robot, vector<Workbench *> &WBProductReady);
double Sell(Robot *Robot, vector<Workbench *> &WBProductLoss);
double MoveSell(Robot *Robot);
int SellFindVectorWBMinID(Robot *Robot, vector<Workbench *> &WBProductLoss);
double operator-(const vector<double> _left, const vector<double> _right);

int main()
{
  MapInit();
  puts("OK");
  fflush(stdout);
  int frameID;
  int WorkBenchNum;
  int i;
  vector<double> AngleSpeed(4, 0.0);
  vector<double> LineSpeed(4, 0.0);
  while (scanf("%d", &frameID) != EOF)
  {
    cin >> Money;
    cin >> WorkBenchNum;
    // cerr << "frameID " << frameID << " Money " << Money <<" WorkBenchNum " << WorkBenchNum << endl;
    ReadInfo(WorkBenchNum, WorkBenchVec, RobotVec);
    vector<Workbench *> WBProductReady;
    vector<Workbench *> WBProductLoss;
    vector<vector<Workbench *>> WorkBenchLossSelf(10);
    // 判断工作台产品格状态， 放入不同队列
    for (i = 0; i < WorkBenchNum; i++)
    {
      if (WorkBenchVec[i]->ProductStatus == 0)
        WBProductLoss.push_back(WorkBenchVec[i]);
      else
        WBProductReady.push_back(WorkBenchVec[i]);
    }
    // 如果产品没有生产的工作台数不为0，那么分为9个不同类型的工作台组
    if (!WBProductLoss.empty())
    {
      WorkBenchAllocate(WBProductLoss, WorkBenchLossSelf);
    }

    printf("%d\n", frameID);

    // 判断机器人当前状态，根据不同状态进入不同选择
    for (i = 0; i < 4; i++)
    { // 未持有物品，且无目标工作台，且有产品的工作台数不为0 -> 进去Buy 寻找目标工作台
      if (RobotVec[i]->TypeArticleCarry == 0 && RobotVec[i]->HaveTarget == -1 && WBProductReady.size() != 0)
      {
        AngleSpeed[i] = Buy(RobotVec[i], WBProductReady);
        cerr << "if1" << endl;
      }
      // 未持有物品，但有目标工作台 -> MoveBuy 向目标工作台移动，达到位置时购买
      else if (RobotVec[i]->TypeArticleCarry == 0 && RobotVec[i]->HaveTarget != -1)
      {
        cerr << "if2" << endl;
        AngleSpeed[i] = MoveBuy(RobotVec[i]);
      }
      // 持有物品，类型为TypeArticleCarry，但无目标工作台 -> Sell 寻找可以售卖的工作台
      else if (RobotVec[i]->TypeArticleCarry != 0 && RobotVec[i]->HaveTarget == -1 && WBProductLoss.size() != 0)
      {
        cerr << "if3" << endl;
        AngleSpeed[i] = Sell(RobotVec[i], WBProductLoss);
      }
      // 持有物品，类型为TypeArticleCarry，且有目标工作台 -> MoveSell 向目标工作台移动，达到位置时出售
      else if (RobotVec[i]->TypeArticleCarry != 0 && RobotVec[i]->HaveTarget != -1)
      {
        cerr << "if4" << endl;
        AngleSpeed[i] = MoveSell(RobotVec[i]);
      }
      // if (RobotVec[i]->TypeArticleCarry != 0 && RobotVec[i]->HaveTarget == -1)
      // {
      //   // RobotVec[i] -> HaveTarget = -1;
      //   cerr << "if5" << endl;
      //   cout << "destroy " << i << endl;
      //   exit(1);
      // }
    }
    for (i = 0; i < 4; i++)
    {
      cerr << i << "target " << RobotVec[i]->HaveTarget << endl;
    }
    for (i = 0; i < 4; i++)
    {
      if (abs(AngleSpeed[i]) < 0.2)
        LineSpeed[i] = 3;
      else
        LineSpeed[i] = 0.5;
    }

    for (int robotId = 0; robotId < 4; robotId++)
    {
      printf("forward %d %lf\n", robotId, LineSpeed[robotId]);
      printf("rotate %d %f\n", robotId, AngleSpeed[robotId]);
    }
    printf("OK\n", frameID);
    fflush(stdout);
  }
}

// 地图初始化
bool MapInit()
{
  char line[1024];
  int i = 0;
  while (fgets(line, sizeof line, stdin))
  {
    if (line[0] == 'O' && line[1] == 'K')
    {
      cerr << "WorkBenchVec.size = " << WorkBenchVec.size() << endl;
      for (i = 0; i < RobotNum; i++)
      {
        RobotVec[i]->RobotID = i;
        RobotVec[i]->ShowRobot();
      }
      return true;
    }
    map[i] = line;
    // cerr << map[i] << endl;
    // exit(1);
    WorkBenchAndRobotRead(map[i], WorkBenchVec, RobotVec, i);
    i++; // iterative updates
  }
  cerr << "An error occurred with the readUntilOK function." << endl;
  exit(1);
  return false;
}

// 读取判题器输出
bool ReadInfo(int _WorkBenchNum, vector<Workbench *> _WorkBenchVec, vector<Robot *> _RobotVec)
{
  char line[10];
  int i = 0;
  vector<double> _axis(2, 0.0);
  vector<double> _speed(2, 0.0);
#if (SHOW == 1)
  cerr << "----------------------------------------------" << endl;
  cerr << "WorkBench" << endl;
#endif
  for (i = 0; i < _WorkBenchNum; i++)
  {
    cin >> _WorkBenchVec[i]->GetWorkBenchKind();
    cin >> _axis[0] >> _axis[1];
    _WorkBenchVec[i]->GetAxis() = _axis;
    cin >> _WorkBenchVec[i]->GetRestWorkTime() >> _WorkBenchVec[i]->GetMaterialStatus() >> _WorkBenchVec[i]->GetProductStatus();
// cerr << "Material Status" << _WorkBenchVec[i]->MaterialStatus << endl;
#if (SHOW == 1)
    _WorkBenchVec[i]->ShowWorkBench();
#endif
  }
#if (SHOW == 1)
  cerr << "----------------------------------------------" << endl;
  cerr << "Robot" << endl;
#endif
  for (i = 0; i < 4; i++)
  {
    cin >> _RobotVec[i]->GetWorkBenchID() >> _RobotVec[i]->GetTypeArticleCarry() >> _RobotVec[i]->GetTimeValueCoeffcient() >> _RobotVec[i]->GetCollisionValueCoeffcient() >> _RobotVec[i]->GetPalstance();
    cin >> _speed[0] >> _speed[1];
    _RobotVec[i]->SetLinearSpeed(_speed);
    cin >> _RobotVec[i]->GetTowards();
    cin >> _axis[0] >> _axis[1];
    _RobotVec[i]->GetAxis() = _axis;
#if (SHOW == 1)
    _RobotVec[i]->ShowRobot();
#endif
  }
  cin.get();
  if (fgets(line, sizeof line, stdin))
  {
    cerr << line << endl;
    if (line[0] == 'O' && line[1] == 'K')
    {
      return true;
    }
    else
    {
      cerr << "Get infomation error." << endl;
      exit(1);
    }
  }
  return false;
}

// 根据最近原则，寻找机器人买物品的目标工作台
double Buy(Robot *Robot, vector<Workbench *> &WBProductReady)
{
  int WBMinID;
  WBMinID = BuyFindVectorWBMinID(Robot, WBProductReady);
  Robot->HaveTarget = WBMinID;
  WorkBenchVec[WBMinID]->LockBuy = 1;
  // WBProductReady.erase(WBProductReady.begin() + ID[0]);
  return MoveBuy(Robot);
}

// 机器人向目标工作台移动，并检测是否到达，到达直接购买商品
double MoveBuy(Robot *Robot)
{
  int Target;
  Target = Robot->HaveTarget;
  // 如果当前已经到达目标工作台，则此帧不移动，购买商品
  if (Target == Robot->WorkBenchID)
  {
    cout << "buy " << Robot->RobotID << endl;
    fflush(stdout);
    Robot->HaveTarget = -1;
    WorkBenchVec[Target]->LockBuy = 0;
  }
  // 未到达，则继续向目标工作台移动
  else
  {
    int flag = 0;
    int quadrant = 0;
    vector<double> axis = Robot->GetAxis();
    double XDistance = WorkBenchVec[Target]->GetAxis()[0] - axis[0];
    double YDistance = WorkBenchVec[Target]->GetAxis()[1] - axis[1];
    if (YDistance > 0.0 && XDistance > 0.0)
      quadrant = 1;
    else if (YDistance < 0.0 && XDistance > 0.0)
      quadrant = 4;
    else if (YDistance > 0.0 && XDistance < 0.0)
      quadrant = 2;
    else
      quadrant = 3;

    double Angle = atan2(YDistance, XDistance);
    double AngleDifference = abs(Angle - Robot->GetTowards());
    if (abs(AngleDifference) < 0.5)
      flag = Angle - Robot->GetTowards() > 0.0 ? 1 : -1; // 1:ni -1 sun;
    else
      flag = 1;
    double RealDis = sqrt(XDistance * XDistance + YDistance * YDistance);
    Robot->GetWantToCloseWBID() = WorkBenchVec[Target]->GetWorkBenchID();
    Robot->GetRobotWorkBenchDis() = RealDis;
    Robot->GetAngleDifference() = AngleDifference;
    double AngleSpeed = AngleDifference > 1.5 ? 1.5 : AngleDifference;
    return AngleSpeed * flag;
  }
}

// 找到离机器人最近的，且未被买锁定的工作台，返回其全局索引
int BuyFindVectorWBMinID(Robot *Robot, vector<Workbench *> &WBProductReady)
{
  int i;
  int VectorWBMinID = WBProductReady.size() - 1;
  vector<double> axis = Robot->GetAxis();
  double MinDistance = 1e10;
  for (i = 0; i < WBProductReady.size(); i++)
  {
    double DistanceTmp = axis - WBProductReady[i]->GetAxis();
    if (DistanceTmp < MinDistance && WBProductReady[i]->LockBuy == 0)
    {
      MinDistance = DistanceTmp;
      VectorWBMinID = i;
    }
  }
  return WBProductReady[VectorWBMinID]->WorkBenchID;
}

// 根据最近原则，寻找机器人卖物品的目标工作台
double Sell(Robot *Robot, vector<Workbench *> &WBProductLoss)
{
  int WBMinID;
  WBMinID = SellFindVectorWBMinID(Robot, WBProductLoss);
  Robot->HaveTarget = WBMinID;
  WorkBenchVec[WBMinID]->LockSell = 1;
  return MoveSell(Robot);
}

// 机器人向目标工作台移动，并检测是否到达，到达直接出售商品
double MoveSell(Robot *Robot)
{
  int Target;
  Target = Robot->HaveTarget;
  // 如果当前已经到达目标工作台，则此帧不移动，出售商品
  if (Target == Robot->WorkBenchID)
  {
    cout << "sell " << Robot->RobotID << endl;
    fflush(stdout);
    Robot->HaveTarget = -1;
    WorkBenchVec[Target]->LockSell = 0;
  }
  // 未到达，则继续向目标工作台移动
  else
  {
    int flag = 0;
    int quadrant = 0;
    vector<double> axis = Robot->GetAxis();
    double XDistance = WorkBenchVec[Target]->GetAxis()[0] - axis[0];
    double YDistance = WorkBenchVec[Target]->GetAxis()[1] - axis[1];
    if (YDistance > 0.0 && XDistance > 0.0)
      quadrant = 1;
    else if (YDistance < 0.0 && XDistance > 0.0)
      quadrant = 4;
    else if (YDistance > 0.0 && XDistance < 0.0)
      quadrant = 2;
    else
      quadrant = 3;

    double Angle = atan2(YDistance, XDistance);
    double AngleDifference = abs(Angle - Robot->GetTowards());
    if (abs(AngleDifference) < 0.5)
      flag = Angle - Robot->GetTowards() > 0.0 ? 1 : -1; // 1:ni -1 sun;
    else
      flag = 1;
    double RealDis = sqrt(XDistance * XDistance + YDistance * YDistance);
    Robot->GetWantToCloseWBID() = WorkBenchVec[Target]->GetWorkBenchID();
    Robot->GetRobotWorkBenchDis() = RealDis;
    Robot->GetAngleDifference() = AngleDifference;
    double AngleSpeed = AngleDifference > 1.5 ? 1.5 : AngleDifference;
    return AngleSpeed * flag;
  }
}

// 找到离机器人最近的，且未被买锁定的工作台，返回其全局索引
int SellFindVectorWBMinID(Robot *Robot, vector<Workbench *> &WBProductLoss)
{
  int i;
  int ProductID = Robot->TypeArticleCarry; // 当前机器人携带的物品类型
  int VectorWBMinID = WBProductLoss.size() - 1;
  vector<double> axis = Robot->GetAxis();
  double MinDistance = 1e10;
  switch (ProductID)
  {
  case 1:
  {
    for (i = 0; i < WBProductLoss.size(); i++)
    {
      int flag = 0;
      int Material = WBProductLoss[i]->MaterialStatus;
      double DistanceTmp = axis - WBProductLoss[i]->GetAxis();
      if (Material == 0)
      {
        flag = 0;
      }
      else
      {
        if (Material & (1 << ProductID))
        {
          continue;
        }
        else
        {
          flag = 0;
        }
      }
      if (DistanceTmp < MinDistance && WBProductLoss[i]->LockSell == 0 && flag == 0 && (WBProductLoss[i]->WorkBenchKind == 4 || WBProductLoss[i]->WorkBenchKind == 5 || WBProductLoss[i]->WorkBenchKind == 9))
      {
        MinDistance = DistanceTmp;
        VectorWBMinID = i;
      }
    }
    return WBProductLoss[VectorWBMinID]->WorkBenchID;
    // 如果一种类型的工作台全部用完，应该设置销毁。1.设置销毁位2.随机选一个3.等待几帧走远了再找当时最近的
  }
  case 2:
  {
    for (i = 0; i < WBProductLoss.size(); i++)
    {
      int flag = 0;
      int Material = WBProductLoss[i]->MaterialStatus;
      double DistanceTmp = axis - WBProductLoss[i]->GetAxis();
      if (Material == 0)
      {
        flag = 0;
      }
      else
      {
        if (Material & (1 << ProductID))
        {
          continue;
        }
        else
        {
          flag = 0;
        }
      }
      if (DistanceTmp < MinDistance && WBProductLoss[i]->LockSell == 0 && flag == 0 && (WBProductLoss[i]->WorkBenchKind == 4 || WBProductLoss[i]->WorkBenchKind == 6 || WBProductLoss[i]->WorkBenchKind == 9))
      {
        MinDistance = DistanceTmp;
        VectorWBMinID = i;
      }
    }
    return WBProductLoss[VectorWBMinID]->WorkBenchID;
  }
  case 3:
  {
    for (i = 0; i < WBProductLoss.size(); i++)
    {
      int flag = 0;
      int Material = WBProductLoss[i]->MaterialStatus;
      double DistanceTmp = axis - WBProductLoss[i]->GetAxis();
      if (Material == 0)
      {
        flag = 0;
      }
      else
      {
        if (Material & (1 << ProductID))
        {
          continue;
        }
        else
        {
          flag = 0;
        }
      }
      if (DistanceTmp < MinDistance && WBProductLoss[i]->LockSell == 0 && flag == 0 && (WBProductLoss[i]->WorkBenchKind == 5 || WBProductLoss[i]->WorkBenchKind == 6 || WBProductLoss[i]->WorkBenchKind == 9))
      {
        MinDistance = DistanceTmp;
        VectorWBMinID = i;
      }
    }
    return WBProductLoss[VectorWBMinID]->WorkBenchID;
  }
  case 4:
  {
    for (i = 0; i < WBProductLoss.size(); i++)
    {
      int flag = 0;
      int Material = WBProductLoss[i]->MaterialStatus;
      double DistanceTmp = axis - WBProductLoss[i]->GetAxis();
      if (Material == 0)
      {
        flag = 0;
      }
      else
      {
        if (Material & (1 << ProductID))
        {
          continue;
        }
        else
        {
          flag = 0;
        }
      }
      if (DistanceTmp < MinDistance && WBProductLoss[i]->LockSell == 0 && flag == 0 && (WBProductLoss[i]->WorkBenchKind == 7 || WBProductLoss[i]->WorkBenchKind == 9))
      {
        MinDistance = DistanceTmp;
        VectorWBMinID = i;
      }
    }
    return WBProductLoss[VectorWBMinID]->WorkBenchID;
  }
  case 5:
  {
    for (i = 0; i < WBProductLoss.size(); i++)
    {
      int flag = 0;
      int Material = WBProductLoss[i]->MaterialStatus;
      double DistanceTmp = axis - WBProductLoss[i]->GetAxis();
      if (Material == 0)
      {
        flag = 0;
      }
      else
      {
        if (Material & (1 << ProductID))
        {
          continue;
        }
        else
        {
          flag = 0;
        }
      }
      if (DistanceTmp < MinDistance && WBProductLoss[i]->LockSell == 0 && flag == 0 && (WBProductLoss[i]->WorkBenchKind == 7 || WBProductLoss[i]->WorkBenchKind == 9))
      {
        MinDistance = DistanceTmp;
        VectorWBMinID = i;
      }
    }
    return WBProductLoss[VectorWBMinID]->WorkBenchID;
  }
  case 6:
  {
    for (i = 0; i < WBProductLoss.size(); i++)
    {
      int flag = 0;
      int Material = WBProductLoss[i]->MaterialStatus;
      double DistanceTmp = axis - WBProductLoss[i]->GetAxis();
      if (Material == 0)
      {
        flag = 0;
      }
      else
      {
        if (Material & (1 << ProductID))
        {
          continue;
        }
        else
        {
          flag = 0;
        }
      }
      if (DistanceTmp < MinDistance && WBProductLoss[i]->LockSell == 0 && flag == 0 && (WBProductLoss[i]->WorkBenchKind == 7 || WBProductLoss[i]->WorkBenchKind == 9))
      {
        MinDistance = DistanceTmp;
        VectorWBMinID = i;
      }
    }
    return WBProductLoss[VectorWBMinID]->WorkBenchID;
  }
  case 7:
  {
    for (i = 0; i < WBProductLoss.size(); i++)
    {
      int flag = 0;
      int Material = WBProductLoss[i]->MaterialStatus;
      double DistanceTmp = axis - WBProductLoss[i]->GetAxis();
      if (Material == 0)
      {
        flag = 0;
      }
      else
      {
        if (Material & (1 << ProductID))
        {
          continue;
        }
        else
        {
          flag = 0;
        }
      }
      if (DistanceTmp < MinDistance && WBProductLoss[i]->LockSell == 0 && flag == 0 && (WBProductLoss[i]->WorkBenchKind == 8 || WBProductLoss[i]->WorkBenchKind == 9))
      {
        MinDistance = DistanceTmp;
        VectorWBMinID = i;
      }
    }
    return WBProductLoss[VectorWBMinID]->WorkBenchID;
  }
  }
}

double operator-(const vector<double> _left, const vector<double> _right)
{
  return sqrt((_left[0] - _right[0]) * (_left[0] - _right[0]) +
              (_left[1] - _right[1]) * (_left[1] - _right[1]));
}
