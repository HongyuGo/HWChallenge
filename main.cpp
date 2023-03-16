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
double Buy(Robot *Robot, vector<vector<Workbench *>> &WorkBenchReadySelf);
double MoveBuy(Robot *Robot);
int BuyFindVectorWBMinID(Robot *Robot, vector<vector<Workbench *>> &WorkBenchReadySelf);
double Sell(Robot *Robot);
double MoveSell(Robot *Robot);
int SellFindVectorWBMinID(Robot *Robot);
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
    vector<vector<Workbench *>> WorkBenchReadySelf(10);
    for (int i = 0; i < WorkBenchVec.size(); i++)
    {
      WorkBenchVec[i]->LockSell = 0;
    }
    // 判断工作台产品格状态， 放入不同队列
    for (i = 0; i < WorkBenchNum; i++)
    {
      if (WorkBenchVec[i]->ProductStatus == 0)
        WBProductLoss.push_back(WorkBenchVec[i]);
      else
        WBProductReady.push_back(WorkBenchVec[i]);
    }
    // 如果产品没有生产的工作台数不为0，那么分为9个不同类型的工作台组
    if (!WBProductReady.empty())
    {
      WorkBenchAllocate(WBProductReady, WorkBenchReadySelf);
    }

    printf("%d\n", frameID);

    // 判断机器人当前状态，根据不同状态进入不同选择
    for (i = 0; i < 4; i++)
    {
      // 持有物品，类型为TypeArticleCarry，但无目标工作台 -> Sell 寻找可以售卖的工作台
      if (RobotVec[i]->TypeArticleCarry != 0 && RobotVec[i]->HaveTarget == -1)
      {
        cerr << "Sell" << endl;
        AngleSpeed[i] = Sell(RobotVec[i]);
      }
      // 未持有物品，且无目标工作台，且有产品的工作台数不为0 -> 进去Buy 寻找目标工作台
      else if (RobotVec[i]->TypeArticleCarry == 0 && RobotVec[i]->HaveTarget == -1 && WBProductReady.size() != 0)
      {
        cerr << "Buy" << endl;
        AngleSpeed[i] = Buy(RobotVec[i], WorkBenchReadySelf);
      }
      // 未持有物品，但有目标工作台 -> MoveBuy 向目标工作台移动，达到位置时购买
      else if (RobotVec[i]->TypeArticleCarry == 0 && RobotVec[i]->HaveTarget != -1)
      {
        cerr << "MoveBuy" << endl;
        AngleSpeed[i] = MoveBuy(RobotVec[i]);
      }

      // 持有物品，类型为TypeArticleCarry，且有目标工作台 -> MoveSell 向目标工作台移动，达到位置时出售
      else if (RobotVec[i]->TypeArticleCarry != 0 && RobotVec[i]->HaveTarget != -1)
      {
        cerr << "MoveSell" << endl;
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
    cerr << "frameID:" << frameID << endl;
    for (i = 0; i < 4; i++)
    {
      cerr << i << "target " << RobotVec[i]->HaveTarget << endl;
    }
    for (i = 0; i < 4; i++)
    {
      if (abs(AngleSpeed[i]) < 0.2)
        LineSpeed[i] = 5.5;
      else
        LineSpeed[i] = 1;
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
double Buy(Robot *Robot, vector<vector<Workbench *>> &WorkBenchReadySelf)
{
  int WBMinID;
  WBMinID = BuyFindVectorWBMinID(Robot, WorkBenchReadySelf);
  cerr << "my buy target is " << WBMinID << endl;
  Robot->HaveTarget = WBMinID;
  WorkBenchVec[WBMinID]->LockBuy = 1;
  // WBProductReady.erase(WBProductReady.begin() + ID[0]);
  return MoveBuy(Robot);
}

// 机器人向目标工作台移动，并检测是否到达，到达直接购买商品
double MoveBuy(Robot *Robot)
{
  int flagbuy = 0;                                // 移动的路上就检测是否可以卖出去
  int Target = Robot->HaveTarget;                 // 前往购买的工作台ID
  int Kind = WorkBenchVec[Target]->WorkBenchKind; // 前往购买的工作台类型
  // 如果当前已经到达目标工作台，则此帧不移动，购买商品
  if (Target == Robot->WorkBenchID)
  {
    cout << "buy " << Robot->RobotID << endl;
    fflush(stdout);
    Robot->HaveTarget = -1;
    WorkBenchVec[Target]->LockBuy = 0;
    return 0;
  }
  // 未到达，则继续向目标工作台移动
  else
  {
    // 判断当前要去购买的商品是否能被出售
    switch (Kind)
    {
    case 7:
    {
      flagbuy = 1;
      break;
    }
    case 6:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 7 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell6 == 0)
        {
          int Material = WorkBenchVec[k]->MaterialStatus;
          if (Material == 0)
          {
            flagbuy = 1;
            break;
          }
          else
          {
            if (Material & (1 << Kind))
            {
            }
            else
            {
              flagbuy = 1;
              break;
            }
          }
        }
      }
      break;
    }
    case 5:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 7 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell5 == 0)
        {
          int Material = WorkBenchVec[k]->MaterialStatus;
          if (Material == 0)
          {
            flagbuy = 1;
            break;
          }
          else
          {
            if (Material & (1 << Kind))
            {
            }
            else
            {
              flagbuy = 1;
              break;
            }
          }
        }
      }
      break;
    }
    case 4:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 7 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell4 == 0)
        {
          int Material = WorkBenchVec[k]->MaterialStatus;
          if (Material == 0)
          {
            flagbuy = 1;
            break;
          }
          else
          {
            if (Material & (1 << Kind))
            {
            }
            else
            {
              flagbuy = 1;
              break;
            }
          }
        }
      }
      break;
    }
    case 3:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 5 || WorkBenchVec[k]->WorkBenchKind == 6 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell3 == 0)
        {

          int Material = WorkBenchVec[k]->MaterialStatus;
          cerr << "WB" << k << " " << Material << " " << WorkBenchVec[k]->WorkBenchKind << endl;
          if (Material == 0)
          {
            flagbuy = 1;
            break;
          }
          else
          {
            if (Material & (1 << Kind))
            {
            }
            else
            {
              flagbuy = 1;
              break;
            }
          }
        }
      }
      break;
    }
    case 2:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 4 || WorkBenchVec[k]->WorkBenchKind == 6 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell2 == 0)
        {
          int Material = WorkBenchVec[k]->MaterialStatus;
          if (Material == 0)
          {
            flagbuy = 1;
            break;
          }
          else
          {
            if (Material & (1 << Kind))
            {
            }
            else
            {
              flagbuy = 1;
              break;
            }
          }
        }
      }
      break;
    }
    case 1:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 4 || WorkBenchVec[k]->WorkBenchKind == 5 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell1 == 0)
        {
          int Material = WorkBenchVec[k]->MaterialStatus;
          if (Material == 0)
          {
            flagbuy = 1;
            break;
          }
          else
          {
            if (Material & (1 << Kind))
            {
            }
            else
            {
              flagbuy = 1;
              break;
            }
          }
        }
      }
      break;
    }
    }
    if (flagbuy)
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
    else
    {
      Robot->HaveTarget = -1;
      WorkBenchVec[Target]->LockBuy = 0;
      return 0;
    }
  }
}

// 找到离机器人最近的，且未被买锁定的工作台，返回其全局索引
int BuyFindVectorWBMinID(Robot *Robot, vector<vector<Workbench *>> &WorkBenchReadySelf)
{
  int VectorWBMinID = -1;
  vector<double> axis = Robot->GetAxis();
  double MinDistance = 1e10;
  int flag[8] = {0};
  flag[7] = 1;
  int flagout = 0;            // 标志是否找到了能够买的工作台
  for (int i = 1; i < 7; i++) // 判断每种类别的商品是否能被出售
  {
    switch (i)
    {
    case 6:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 7 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell6 == 0 && WorkBenchVec[k]->LockBuy == 0)
        {
          int Material = WorkBenchVec[k]->MaterialStatus;
          if (Material == 0)
          {
            flag[6] = 1;
            break;
          }
          else
          {
            if (Material & (1 << i))
            {
            }
            else
            {
              flag[6] = 1;
              break;
            }
          }
        }
      }
      break;
    }
    case 5:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 7 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell5 == 0 && WorkBenchVec[k]->LockBuy == 0)
        {
          int Material = WorkBenchVec[k]->MaterialStatus;
          if (Material == 0)
          {
            flag[5] = 1;
            break;
          }
          else
          {
            if (Material & (1 << i))
            {
            }
            else
            {
              flag[5] = 1;
              break;
            }
          }
        }
      }
      break;
    }
    case 4:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 7 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell4 == 0 && WorkBenchVec[k]->LockBuy == 0)
        {
          int Material = WorkBenchVec[k]->MaterialStatus;
          if (Material == 0)
          {
            flag[4] = 1;
            break;
          }
          else
          {
            if (Material & (1 << i))
            {
            }
            else
            {
              flag[4] = 1;
              break;
            }
          }
        }
      }
      break;
    }
    case 3:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 5 || WorkBenchVec[k]->WorkBenchKind == 6 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell3 == 0 && WorkBenchVec[k]->LockBuy == 0)
        {

          int Material = WorkBenchVec[k]->MaterialStatus;
          cerr << "WB" << k << " " << Material << " " << WorkBenchVec[k]->WorkBenchKind << endl;
          if (Material == 0)
          {
            flag[3] = 1;
            break;
          }
          else
          {
            if (Material & (1 << i))
            {
            }
            else
            {
              flag[3] = 1;
              break;
            }
          }
        }
      }
      break;
    }
    case 2:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 4 || WorkBenchVec[k]->WorkBenchKind == 6 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell2 == 0 && WorkBenchVec[k]->LockBuy == 0)
        {
          int Material = WorkBenchVec[k]->MaterialStatus;
          if (Material == 0)
          {
            flag[2] = 1;
            break;
          }
          else
          {
            if (Material & (1 << i))
            {
            }
            else
            {
              flag[2] = 1;
              break;
            }
          }
        }
      }
      break;
    }
    case 1:
    {
      for (int k = 0; k < WorkBenchVec.size(); k++)
      {
        if ((WorkBenchVec[k]->WorkBenchKind == 4 || WorkBenchVec[k]->WorkBenchKind == 5 || WorkBenchVec[k]->WorkBenchKind == 9) && WorkBenchVec[k]->LockSell1 == 0 && WorkBenchVec[k]->LockBuy == 0)
        {
          int Material = WorkBenchVec[k]->MaterialStatus;
          if (Material == 0)
          {
            flag[1] = 1;
            break;
          }
          else
          {
            if (Material & (1 << i))
            {
            }
            else
            {
              flag[1] = 1;
              break;
            }
          }
        }
      }
      break;
    }
    }
  }
  for (int i = 1; i < 8; i++)
  {
    cerr << "flag[" << i << "]=" << flag[i] << " ";
  }
  cerr << endl;
  for (int i = 7; i > 0; i--) // 从第7类工作台开始到第1类都能生产物品，优先购买高品类
  {
    if (flag[i])
    {
      for (int j = 0; j < WorkBenchReadySelf[i].size(); j++)
      {
        // 判断当前产品能否拿（拿了能不能卖出去）
        double DistanceTmp = axis - WorkBenchReadySelf[i][j]->GetAxis();
        if (DistanceTmp < MinDistance && WorkBenchReadySelf[i][j]->LockBuy == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = j;
        }
      }

      if (VectorWBMinID != -1)
      {
        cerr << "kind:" << i << " MinID:" << VectorWBMinID << " ggggggggg" << endl;
        return WorkBenchReadySelf[i][VectorWBMinID]->WorkBenchID;
      }
      else
      {
        continue;
      }
      // return WorkBenchReadySelf[i][VectorWBMinID]->WorkBenchID;
    }
  }
}

// 根据最近原则，寻找机器人卖物品的目标工作台
double Sell(Robot *Robot)
{
  int WBMinID = SellFindVectorWBMinID(Robot);
  cerr << "my sell target is " << WBMinID << endl;
  int ProductID = Robot->TypeArticleCarry;
  Robot->HaveTarget = WBMinID;
  switch (ProductID)
  {
  case 1:
  {
    WorkBenchVec[WBMinID]->LockSell1 = 1;
    break;
  }
  case 2:
  {
    WorkBenchVec[WBMinID]->LockSell2 = 1;
    break;
  }
  case 3:
  {
    WorkBenchVec[WBMinID]->LockSell3 = 1;
    break;
  }
  case 4:
  {
    WorkBenchVec[WBMinID]->LockSell4 = 1;
    break;
  }
  case 5:
  {
    WorkBenchVec[WBMinID]->LockSell5 = 1;
    break;
  }
  case 6:
  {
    WorkBenchVec[WBMinID]->LockSell6 = 1;
    break;
  }
  case 7:
  {
    WorkBenchVec[WBMinID]->LockSell7 = 1;
    break;
  }
  }
  return MoveSell(Robot);
}

// 机器人向目标工作台移动，并检测是否到达，到达直接出售商品
double MoveSell(Robot *Robot)
{
  int Target = Robot->HaveTarget;
  int ProductID = Robot->TypeArticleCarry;
  // 如果当前已经到达目标工作台，则此帧不移动，出售商品
  if (Target == Robot->WorkBenchID)
  {
    cout << "sell " << Robot->RobotID << endl;
    fflush(stdout);
    Robot->HaveTarget = -1;
    switch (ProductID)
    {
    case 1:
    {
      WorkBenchVec[Target]->LockSell1 = 0;
      break;
    }
    case 2:
    {
      WorkBenchVec[Target]->LockSell2 = 0;
      break;
    }
    case 3:
    {
      WorkBenchVec[Target]->LockSell3 = 0;
      break;
    }
    case 4:
    {
      WorkBenchVec[Target]->LockSell4 = 0;
      break;
    }
    case 5:
    {
      WorkBenchVec[Target]->LockSell5 = 0;
      break;
    }
    case 6:
    {
      WorkBenchVec[Target]->LockSell6 = 0;
      break;
    }
    case 7:
    {
      WorkBenchVec[Target]->LockSell7 = 0;
      break;
    }
    }
    return 0;
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

// 找到离机器人最近的，且未被卖锁定的工作台，返回其全局索引
int SellFindVectorWBMinID(Robot *Robot)
{
  int i;
  int ProductID = Robot->TypeArticleCarry; // 当前机器人携带的物品类型
  int VectorWBMinID = -1;
  vector<double> axis = Robot->GetAxis();
  double MinDistance = 1e10;
  switch (ProductID)
  {
  case 1:
  {
    for (i = 0; i < WorkBenchVec.size(); i++)
    {
      double DistanceTmp = axis - WorkBenchVec[i]->GetAxis();
      if (WorkBenchVec[i]->WorkBenchKind == 4 || WorkBenchVec[i]->WorkBenchKind == 5)
      {
        int flag = 0;
        int Material = WorkBenchVec[i]->MaterialStatus;
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
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell1 == 0 && flag == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
      else if (WorkBenchVec[i]->WorkBenchKind == 9)
      {
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell1 == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
    }

    return VectorWBMinID;
    // 如果一种类型的工作台全部用完，应该设置销毁。1.设置销毁位2.随机选一个3.等待几帧走远了再找当时最近的
  }
  case 2:
  {
    for (i = 0; i < WorkBenchVec.size(); i++)
    {
      double DistanceTmp = axis - WorkBenchVec[i]->GetAxis();
      if (WorkBenchVec[i]->WorkBenchKind == 4 || WorkBenchVec[i]->WorkBenchKind == 6)
      {
        int flag = 0;
        int Material = WorkBenchVec[i]->MaterialStatus;
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
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell2 == 0 && flag == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
      else if (WorkBenchVec[i]->WorkBenchKind == 9)
      {
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell2 == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
    }
    return VectorWBMinID;
  }
  case 3:
  {
    for (i = 0; i < WorkBenchVec.size(); i++)
    {
      double DistanceTmp = axis - WorkBenchVec[i]->GetAxis();
      if (WorkBenchVec[i]->WorkBenchKind == 5 || WorkBenchVec[i]->WorkBenchKind == 6)
      {
        int flag = 0;
        int Material = WorkBenchVec[i]->MaterialStatus;
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
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell3 == 0 && flag == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
      else if (WorkBenchVec[i]->WorkBenchKind == 9)
      {
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell3 == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
    }
    return VectorWBMinID;
  }
  case 4:
  {
    for (i = 0; i < WorkBenchVec.size(); i++)
    {
      double DistanceTmp = axis - WorkBenchVec[i]->GetAxis();
      if (WorkBenchVec[i]->WorkBenchKind == 7)
      {
        int flag = 0;
        int Material = WorkBenchVec[i]->MaterialStatus;
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
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell4 == 0 && flag == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
      else if (WorkBenchVec[i]->WorkBenchKind == 9)
      {
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell4 == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
    }
    return VectorWBMinID;
  }
  case 5:
  {
    for (i = 0; i < WorkBenchVec.size(); i++)
    {
      double DistanceTmp = axis - WorkBenchVec[i]->GetAxis();
      if (WorkBenchVec[i]->WorkBenchKind == 7)
      {
        int flag = 0;
        int Material = WorkBenchVec[i]->MaterialStatus;
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
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell5 == 0 && flag == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
      else if (WorkBenchVec[i]->WorkBenchKind == 9)
      {
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell5 == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
    }
    return VectorWBMinID;
  }
  case 6:
  {
    for (i = 0; i < WorkBenchVec.size(); i++)
    {
      double DistanceTmp = axis - WorkBenchVec[i]->GetAxis();
      if (WorkBenchVec[i]->WorkBenchKind == 7)
      {
        int flag = 0;
        int Material = WorkBenchVec[i]->MaterialStatus;
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
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell6 == 0 && flag == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
      else if (WorkBenchVec[i]->WorkBenchKind == 9)
      {
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell6 == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
    }
    return VectorWBMinID;
  }
  case 7:
  {
    for (i = 0; i < WorkBenchVec.size(); i++)
    {
      double DistanceTmp = axis - WorkBenchVec[i]->GetAxis();
      if (WorkBenchVec[i]->WorkBenchKind == 8 || WorkBenchVec[i]->WorkBenchKind == 9)
      {
        if (DistanceTmp < MinDistance && WorkBenchVec[i]->LockSell7 == 0)
        {
          MinDistance = DistanceTmp;
          VectorWBMinID = i;
        }
      }
    }
    return VectorWBMinID;
  }
  }
}

double operator-(const vector<double> _left, const vector<double> _right)
{
  return sqrt((_left[0] - _right[0]) * (_left[0] - _right[0]) +
              (_left[1] - _right[1]) * (_left[1] - _right[1]));
}
