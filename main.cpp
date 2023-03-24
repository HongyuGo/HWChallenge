#include "Robot.h"
#include "Workbench.h"
#include <cstring>
#include "Params.h"
#define MAPLEN (100)
string mapInit[MAPLEN];
vector<int> WorkBenchNum(10,0);
vector<Workbench*>WorkBenchVec;
vector<vector<Workbench*>> WorkBenchSelf(10);
vector<Robot*>RobotVec;
int Money;
const double NormalSpeed = 6.0;
vector<double> RobotSetLineSpeed = {NormalSpeed,NormalSpeed,NormalSpeed,NormalSpeed};
vector<double> RobotSetAngleSpeed = {3.0,3.0,3.0,3.0};
int GlobalFrameID = 0;
int time111 = 0;
int timeswap = 0;
int flag = 0;
int mapID = 0;
int DoOnce = 0;


// char map[MAPLEN][MAPLEN];
double CalculateAngleSpeed(Robot* _Robot, vector<Workbench*>& _WorkBenchVec, int MinID);
// bool Buy456(Robot* _Robot, vector<Workbench*>& _WorkBench456);
bool Buy7(Robot* _Robot, vector<Workbench*>& _WorkBench7);
void ShowRobotInfo(vector<Robot*>& _RobotVec);
void FlushProduct(Robot* _Robot, vector<Workbench*>& _WorkBenchVec);
bool All(vector<Workbench*>& _WorkBenchVec, int Type, Robot* _Robot);
bool All7(vector<Workbench*>& _WorkBenchVec, int Type, int* CheckID, int RobotID);
bool IsBuy(int Kind, vector<vector<Workbench*>>& _WorkBenchSellSelf);
double RobotCloseWorkBench(Robot* _Robot, vector<Workbench*>& _WorkBenchVec);
double RobotCloseWall(Robot* _Robot);
void Showyuyue(Workbench* _WorkBench);
bool Robot2Robot(Robot* _Robot, vector<Robot*> _RobotVec);
void Buy456(Robot* _Robot, vector<Workbench*>& _WorkBench456);
bool Loss7(Workbench * _WorkBench, int Type);
int start = 0;
bool MapInit() {
    char line[1024];
    int i = 0;
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            cerr << "WorkBenchVec.size = " << WorkBenchVec.size() << endl;
            WorkBenchAllocate(WorkBenchVec, WorkBenchSelf);
            for(i = 1 ; i < 10; i++){
                cerr << "Workbench" << i << "'s number is " << WorkBenchSelf[i].size() << endl;
            }
            cerr << "------------------------------------------------------------------" << endl;
            cerr << "Robot" << endl;
            for(i = 0; i < RobotNum; i++){
                RobotVec[i]->ShowRobot();
            }
            if(WorkBenchVec[IDCount - 1]->WorkBenchKind == 4){
                mapID = 4;
            }
            if(WorkBenchVec[12]->WorkBenchKind == 4){
                mapID = 2;
                RobotSetLineSpeed[0] = NormalSpeed - 3;
            }
            return true;
        }
        mapInit[i] = line;
        // cerr << map[i] << endl;
        // exit(1);
        WorkBenchAndRobotRead(mapInit[i], WorkBenchVec, RobotVec, i); 
        i++;//iterative updates
    }
    cerr << "An error occurred with the readUntilOK function." << endl;
    exit(1);
    return false;
}
bool readUntilOKtet(){
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        // cerr << line << endl;
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
    }
    return false;
}
bool ReadInfo(int _WorkBenchNum, vector<Workbench*>& _WorkBenchVec, vector<Robot*>& _RobotVec){
    char line[10];
    int i = 0;
    vector<double> _axis(2,0.0);
    vector<double> _speed(2,0.0);
    #if (SHOW == 1)
        cerr << "----------------------------------------------" << endl;
        cerr << "WorkBench" << endl;
    #endif
    for(i = 0; i < _WorkBenchNum; i++){
        cin >> _WorkBenchVec[i]->GetWorkBenchKind();
        cin >> _axis[0] >> _axis[1];
        _WorkBenchVec[i] -> GetAxis() = _axis;
        cin >> _WorkBenchVec[i] -> GetRestWorkTime() >> _WorkBenchVec[i] -> GetMaterialStatus() >> _WorkBenchVec[i] -> GetProductStatus();
        _WorkBenchVec[i]->dec2bin(_WorkBenchVec[i]->MaterialStatus, _WorkBenchVec[i]->LookMaterial);
        #if (SHOW == 1)
            _WorkBenchVec[i] -> ShowWorkBench();
        #endif
    }
    #if (SHOW == 1)
        cerr << "----------------------------------------------" << endl;
        cerr << "Robot" << endl;
    #endif
    for(i = 0; i < 4; i++){
        cin >> _RobotVec[i] -> GetWorkBenchID() >> _RobotVec[i] -> GetTypeArticleCarry() >> _RobotVec[i] -> GetTimeValueCoeffcient()
            >> _RobotVec[i] -> GetCollisionValueCoeffcient() >> _RobotVec[i] -> GetPalstance();
        cin >> _speed[0] >> _speed[1];
        _RobotVec[i] -> SetLinearSpeed(_speed);
        cin >> _RobotVec[i] -> GetTowards();
        cin >> _axis[0] >> _axis[1];
        _RobotVec[i] -> GetAxis() = _axis;
        #if (SHOW == 1)
            _RobotVec[i] -> ShowRobot();
        #endif
    }
    cin.get();
    if(fgets(line, sizeof line, stdin)) {
        cerr << line << endl;
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
        else{
            cerr << "Get infomation error." << endl;
            exit(1);
        }
    }
    return false;
}
double operator-(const vector<double> _left, const vector<double> _right){
    return sqrt((_left[0] - _right[0]) * (_left[0] - _right[0]) + (_left[1] - _right[1]) * (_left[1] - _right[1]));
}
ostream& operator<<(ostream& os, const vector<double> _axis){
    os << "_axisX : " << _axis[0] << "_axisY : " << _axis[1];
    return os;
}

// int FindMinDistanceTargetWB(Robot* _Robot, vector<Workbench*>& _WorkBenchVec){
//     int i;
//     int Type = _Robot->TypeArticleCarry;
//     auto cmp = [Type, _Robot](Workbench *a1, Workbench* b1){
//         if(count(a1->LookMaterial.begin(),a1->LookMaterial.end(), 1) > count(b1->LookMaterial.begin(),b1->LookMaterial.end(),1)){
//             return true;
//         }else if(count(a1->LookMaterial.begin(),a1->LookMaterial.end(),1) == count(b1->LookMaterial.begin(),b1->LookMaterial.end(),1)){
//             if(abs(a1 -> RestWorkTime) < abs(b1 -> RestWorkTime)){
//                 return true;
//             }else if(abs(a1->RestWorkTime) == abs(b1->RestWorkTime)){
//                 if(a1->Axis - _Robot->Axis < b1->Axis - _Robot->Axis){
//                     return true;
//                 }else{
//                     return false;
//                 }
//             }else{
//                 return false;
//             }
//         }else{
//             return false;
//         }
//     };
//     map<Workbench*,int,decltype(cmp)> Order(cmp);
//     for(i = 0; i < _WorkBenchVec.size(); i++){
//         if(!_WorkBenchVec[i]->HaveLock(_Robot->TypeArticleCarry, RobotVec, _Robot -> RobotID)){
//             Order.insert(pair<Workbench*,int>(_WorkBenchVec[i],i));
//         }
//     }
//     return Order.begin()->second;
// }

int FindMinDistanceTargetWB(Robot* _Robot, vector<Workbench*>& _WorkBenchVec){
    int i;
    int MinID = _WorkBenchVec.size() + 1;
    vector<double> _axis = _Robot->GetAxis();
    double MinDistance = 1e10;
    int MaterialCount = count(_WorkBenchVec[0]->LookMaterial.begin(),_WorkBenchVec[0]->LookMaterial.end(),1);
    for(i = 0; i < _WorkBenchVec.size(); i++){
        double DistanceTmp = _axis - _WorkBenchVec[i]->GetAxis();
        if(!(_WorkBenchVec[i]->HaveLock(_Robot->TypeArticleCarry, RobotVec, _Robot -> RobotID))){

            if(_WorkBenchVec[0]->WorkBenchKind == 1 || _WorkBenchVec[0]->WorkBenchKind == 2 || _WorkBenchVec[0]->WorkBenchKind == 3){
                if(DistanceTmp < MinDistance){
                        MinDistance = DistanceTmp;
                        MinID= i;
                }
            }else{
                if(_WorkBenchVec[i]->ProductStatus == 1){
                    MinID = i;
                    break;
                }
                if(count(_WorkBenchVec[i]->LookMaterial.begin(),_WorkBenchVec[i]->LookMaterial.end(), 1) > MaterialCount){
                    MinID = i;
                    MaterialCount = count(_WorkBenchVec[i]->LookMaterial.begin(),_WorkBenchVec[i]->LookMaterial.end(), 1);
                }else if(count(_WorkBenchVec[i]->LookMaterial.begin(),_WorkBenchVec[i]->LookMaterial.end(), 1) == MaterialCount){
                    if(DistanceTmp < MinDistance){
                        MinDistance = DistanceTmp;
                        MinID= i;
                    }
                }
            }
        }
        // if(DistanceTmp < MinDistance && !(_WorkBenchVec[i]->HaveLock(_Robot->TypeArticleCarry, RobotVec, _Robot -> RobotID))){
        //     MinDistance = DistanceTmp;
        //     MinID= i;
        // }
    }
    _Robot->FindStatus = 0;
    if(MinID == _WorkBenchVec.size() + 1){
        _Robot->FindStatus = 1;
        MinID = _WorkBenchVec.size() - 1;
    }
    if(_WorkBenchVec[MinID]->RobotScheduled.empty() || find(_WorkBenchVec[MinID]->RobotScheduled.begin(), _WorkBenchVec[MinID]->RobotScheduled.end(), _Robot->RobotID) == _WorkBenchVec[MinID]->RobotScheduled.end()){
        if(_Robot->FindStatus == 0)
            _WorkBenchVec[MinID] -> RobotScheduled.push_back(_Robot->RobotID);
    }
    return MinID;
}


// int FindMinDistanceTargetWB(Robot* _Robot, vector<Workbench*>& _WorkBenchVec){
//     int i;
//     int MinID = _WorkBenchVec.size() + 1;
//     vector<double> _axis = _Robot->GetAxis();
//     double MinDistance = 1e10;
//     for(i = 0; i < _WorkBenchVec.size(); i++){
//         double DistanceTmp = _axis - _WorkBenchVec[i]->GetAxis();
//         if(DistanceTmp < MinDistance && !(_WorkBenchVec[i]->HaveLock(_Robot->TypeArticleCarry, RobotVec, _Robot -> RobotID))){
//             MinDistance = DistanceTmp;
//             MinID= i;
//         }
//     }
//     _Robot->FindStatus = 0;
//     if(MinID == _WorkBenchVec.size() + 1){
//         _Robot->FindStatus = 1;
//         MinID = _WorkBenchVec.size() - 1;
//     }
//     if(_WorkBenchVec[MinID]->RobotScheduled.empty() || find(_WorkBenchVec[MinID]->RobotScheduled.begin(), _WorkBenchVec[MinID]->RobotScheduled.end(), _Robot->RobotID) == _WorkBenchVec[MinID]->RobotScheduled.end()){
//         if(_Robot->FindStatus == 0)
//             _WorkBenchVec[MinID] -> RobotScheduled.push_back(_Robot->RobotID);
//     }
//     return MinID;
// }
double CalculateDistance(Robot* _Robot, vector<Workbench*>& _WorkBenchVec){
    int i;
    int MinID;
    if(_Robot->HaveTarget == -1){
        MinID = FindMinDistanceTargetWB(_Robot, _WorkBenchVec);
        _Robot->HaveTarget = MinID;
    }
    else    
        MinID = _Robot->HaveTarget;
    _Robot->GetWantToCloseWBID() = _WorkBenchVec[MinID]->GetWorkBenchID();
    return CalculateAngleSpeed(_Robot, _WorkBenchVec, MinID);
}

double CalculateAngleSpeed(Robot* _Robot, vector<Workbench*>& _WorkBenchVec, int MinID){
    int flag = 0;
    vector<double> _axis = _Robot->GetAxis();
    double XDistance =  _WorkBenchVec[MinID]->GetAxis()[0] - _axis[0];
    double YDistance =  _WorkBenchVec[MinID]->GetAxis()[1] - _axis[1];
    int quadrant = 0;
    if(YDistance > 0.0 && XDistance > 0.0)
        quadrant = 1;
    else if(YDistance < 0.0 && XDistance > 0.0)
        quadrant = 4;
    else if(YDistance > 0.0 && XDistance < 0.0)
        quadrant = 2;
    else
        quadrant = 3;

    double Angle = atan2(YDistance, XDistance);
    double AngleDifference = abs(Angle - _Robot->GetTowards());
    if(abs(AngleDifference) < 0.4)
        flag = Angle - _Robot->GetTowards() > 0.0 ? 1 : -1; // 1:ni -1 sun;
    else    
        flag = 1;
    if(abs(AngleDifference) >= 0.4){
        AngleDifference = RobotSetAngleSpeed[_Robot->RobotID];
    }
    if(Robot2Robot(_Robot, RobotVec)){
        _Robot->AngleFrameIDFlag = 1;
        _Robot->AngleFrameID = GlobalFrameID;
    }
    if(_Robot->AngleFrameIDFlag == 1 && GlobalFrameID - _Robot->AngleFrameID < 40 && GlobalFrameID > 200){
        // cerr << _Robot->Towards << endl;
        AngleDifference = abs(_Robot->Towards) + PI / 2.0;
        flag = -1;
        // cerr << AngleDifference << endl;
        time111++;
    }else{
        _Robot->AngleFrameIDFlag = 0;
    }
    if(abs(Angle) > 3.0 && abs(_Robot->Towards) > 3)
        flag = 0;
    double AngleSpeed = AngleDifference;
    return AngleSpeed * flag;
}


double Move(Robot* _Robot, int _WorkBenchID, vector<Workbench*>& _WorkBenchVec){
    return CalculateAngleSpeed(_Robot, _WorkBenchVec, _WorkBenchID);
}

int main() {
    MapInit();
    puts("OK");
    fflush(stdout);
    int frameID;
    int WorkBenchNum;
    int i;
    const int Ordernum = 4;
    vector<int> Order0 = {1, 4, 2, 4};
    vector<int> Order1 = {1, 5, 3, 5};
    vector<int> Order2 = {2, 6, 3, 6};
    vector<int> Order3 = {3, 6, 2, 6};
    vector<int> Cur(4,0);
    vector<double> AngleSpeed(4, 0.0);
    vector<vector<int>>Order;
    Order.push_back(Order0);
    Order.push_back(Order1);
    Order.push_back(Order2);
    Order.push_back(Order3);
    vector<double> LineSpeed(4,0.0);
    while (scanf("%d", &frameID) != EOF) {
        GlobalFrameID = frameID;
        cin >> Money;
        cin >> WorkBenchNum;
        // cerr << "frameID " << frameID << " Money " << Money <<" WorkBenchNum " << WorkBenchNum << endl;
        ReadInfo(WorkBenchNum, WorkBenchVec, RobotVec);
        vector<Workbench*> WorkBench456;
        vector<Workbench*> WorkBench7;
        vector<Workbench*> WorkBench6;
        vector<Workbench*> WorkBench5;
        vector<Workbench*> WorkBench4;
        vector<vector<Workbench*>> WorkBenchSellSelf(10);
        int count4 = 0, count5 = 0, count6 = 0;
        for(i = 0; i < WorkBenchNum; i++){
            int CheckID;
            if(WorkBenchVec[i] -> ProductStatus == 1 && (WorkBenchVec[i] -> WorkBenchKind == 4 || WorkBenchVec[i] -> WorkBenchKind == 5 || WorkBenchVec[i] -> WorkBenchKind == 6 )){
                // WorkBench456.push_back(WorkBenchVec[i]);
            }
            if(WorkBenchVec[i] -> ProductStatus == 1 && WorkBenchVec[i] -> WorkBenchKind == 7){
                WorkBench7.push_back(WorkBenchVec[i]);
            }
            if(WorkBenchVec[i] -> ProductStatus == 1 && WorkBenchVec[i] -> WorkBenchKind == 6 && WorkBenchVec[i] -> RobotScheduled.empty() ){
                count6++;
                if(Loss7(WorkBenchVec[i],6)){
                    WorkBench6.push_back(WorkBenchVec[i]);
                    WorkBench456.push_back(WorkBenchVec[i]);
                }
            }
            if(WorkBenchVec[i] -> ProductStatus == 1 && WorkBenchVec[i] -> WorkBenchKind == 5 && WorkBenchVec[i] -> RobotScheduled.empty() ){
                count5++;
                if(Loss7(WorkBenchVec[i],5)){
                    WorkBench5.push_back(WorkBenchVec[i]);
                    WorkBench456.push_back(WorkBenchVec[i]);
                }
            }
            if(WorkBenchVec[i] -> ProductStatus == 1 && WorkBenchVec[i] -> WorkBenchKind == 4 && WorkBenchVec[i] -> RobotScheduled.empty() ){
                count4++;
                if(Loss7(WorkBenchVec[i],4)){
                    WorkBench4.push_back(WorkBenchVec[i]);
                    WorkBench456.push_back(WorkBenchVec[i]);
                }
            }
        }
        cerr << "6 : " << count6 << " 5 : " << count5 << " 4 : " <<count4 << endl;
        map<int, int>Find456Min;
        Find456Min.insert(pair<int,int>(count6,6));
        Find456Min.insert(pair<int,int>(count5,5));
        Find456Min.insert(pair<int,int>(count4,4));
        // WorkBenchSellSelf[4] = WorkBench4;
        // WorkBenchSellSelf[5] =WorkBench5;
        // WorkBenchSellSelf[6] = WorkBench6;
        // int C = 0;
        // if(count4 <= count5){
        //     if(count4 <= count6)
        //         C = 4;
        //     else    
        //         C = 6;
        // }
        // else{
        //     if(count5 <= count6)
        //         C = 5;
        //     else
        //         C = 6;
        // }
        if(mapID == 4){
            if(frameID % 1000 == 0){
                DoOnce = 0;
                flag++;
            }
            if((flag % 4 == 0|| flag%4 == 1) && RobotVec[3]->TypeArticleCarry == 0 && DoOnce == 0){
                Order[3] = Order[0];
                swap(Order[3][0], Order[3][2]);
                DoOnce = 1;
            }
            if(flag % 4 == 2 && RobotVec[3]->TypeArticleCarry == 0 && DoOnce == 0){
                Order[3] = Order[1];
                swap(Order[3][0], Order[3][2]);
                DoOnce = 1;
            }
            if(flag % 4 == 3 && RobotVec[3]->TypeArticleCarry == 0 && DoOnce == 0){
                Order[3] = Order[2];
                swap(Order[3][0], Order[3][2]);
                DoOnce = 1;
            }
        }else if(mapID == 2){
            if(frameID % 1000 == 0){
                DoOnce = 0;
                flag++;
            }
            if(flag % 2 == 0 && RobotVec[3]->TypeArticleCarry == 0 && DoOnce == 0){
                Order[3] = Order[2];
                swap(Order[3][0], Order[3][2]);
                DoOnce = 1;
            }
            if(flag % 2 == 1 && RobotVec[3]->TypeArticleCarry == 0 && DoOnce == 0){
                Order[3] = Order[1];
                swap(Order[3][0], Order[3][2]);
                DoOnce = 1;
            }
        }else{
            if(frameID % 1000 == 0){
                DoOnce = 0;
                flag++;
            }
            if(flag % 3 == 0 && RobotVec[3]->TypeArticleCarry == 0 && DoOnce == 0){
                Order[3] = Order[2];
                swap(Order[3][0], Order[3][2]);
                DoOnce = 1;
            }
            if(flag % 3 == 1 && RobotVec[3]->TypeArticleCarry == 0 && DoOnce == 0){
                Order[3] = Order[1];
                swap(Order[3][0], Order[3][2]);
                DoOnce = 1;
            }
            if(flag % 3 == 2 && RobotVec[3]->TypeArticleCarry == 0 && DoOnce == 0){
                Order[3] = Order[0];
                swap(Order[3][0], Order[3][2]);
                DoOnce = 1;
            }
        }
        cerr << "mapId " << mapID << endl;
        cerr << "flag " << flag << endl;
        // int C = Find456Min.begin()->second;
        // Order[3][1] = C;
        // Order[3][3] = C;
        // Order[3][0] = Order[C - 4][0];
        // Order[3][2] = Order[C - 4][2];
        // cerr << "C" << C << endl;
        // cerr << "timeswap" << timeswap << endl;
        for(i = 0; i < 4; i++){
            // if(flag == 3){}
            // Buy456(RobotVec[i], WorkBench456);
            if(RobotVec[i] -> RobotMode == 3){
                // Cur[i] = 0;
                AngleSpeed[i] = Move(RobotVec[i], RobotVec[i]->WantToCloseWBID, WorkBenchVec);              
                continue;
            }
            // FlushProduct(RobotVec[i], WorkBenchVec);
            if(RobotVec[i] -> TypeArticleCarry == 4 || RobotVec[i] -> TypeArticleCarry == 5 || RobotVec[i] -> TypeArticleCarry == 6)
                RobotVec[i] -> RobotMode = 2;
            if(RobotVec[i] -> TypeArticleCarry == 7){
                RobotVec[i] ->RobotMode = 4;
            }
            if(RobotVec[i] -> RobotMode == 0){
                RobotVec[i]->WantTOCloseWBKind = Order[i][Cur[i]];
                // if(All(WorkBenchSelf[Order[i][1]], RobotVec[i]->WantTOCloseWBKind, RobotVec[i]) && RobotVec[i]->DoOnce == 0 || RobotVec[i] ->DoOnce == 2){
                //     timeswap++;
                //     swap(Order[i][0], Order[i][2]);
                //     RobotVec[i]->WantTOCloseWBKind = Order[i][Cur[i]];
                // }
                AngleSpeed[i] =  CalculateDistance(RobotVec[i], WorkBenchSelf[RobotVec[i]->WantTOCloseWBKind]);
            }else if(RobotVec[i] -> RobotMode == 1){
                // Cur[i] = 0;
                AngleSpeed[i] = Move(RobotVec[i], RobotVec[i]->WantToCloseWBID, WorkBenchVec);
            }
            if(RobotVec[i] -> RobotMode == 2){
                if(WorkBenchSelf[7].size() !=0){
                    // Cur[i] = 0;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[7]);
                    
                }
                else if(WorkBenchSelf[9].size() != 0){
                    // Cur[i] = 0;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[9]);
                }
            }
            if(RobotVec[i] -> RobotMode == 4){
                if(WorkBenchSelf[8].size() != 0){
                    // Cur[i] = 0;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[8]);
                }
                else if(WorkBenchSelf[9].size() != 0){
                    // Cur[i] = 0;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[9]);
                }
            }
        }

        ShowRobotInfo(RobotVec);
        // Showyuyue(WorkBenchVec[16]);
        // Showyuyue(WorkBenchVec[14]);
        // Showyuyue(WorkBenchVec[0]);
        // cerr << "time" << time111 << endl;
        printf("%d\n", frameID);
        for(i = 0; i < 4; i++){
            if(abs(AngleSpeed[i]) < 0.4){
                LineSpeed[i] = RobotSetLineSpeed[i];
                LineSpeed[i] = RobotCloseWall(RobotVec[i]);
                if(mapID == 2){
                    LineSpeed[0] = 3;
                }
            }
            else{
                LineSpeed[i] = RobotCloseWorkBench(RobotVec[i], WorkBenchVec);
                if(RobotVec[i]->AngleFrameIDFlag == 1){
                    LineSpeed[i] = RobotSetLineSpeed[i] - 3;
                }
            }
            cerr << "AngleSpeed" << endl;
            cerr << AngleSpeed[i] << ' ' << endl;
        }
        for(int robotId = 0; robotId < 4; robotId++){
            printf("forward %d %lf\n", robotId, LineSpeed[robotId]);
            printf("rotate %d %f\n", robotId, AngleSpeed[robotId]);
            int BuySellFlag = 0;
            if(RobotVec[robotId]->GetWorkBenchID() == RobotVec[robotId]->GetWantToCloseWBID() && RobotVec[robotId]->TypeArticleCarry != 0){
                cout << "sell " << robotId << endl;
                fflush(stdout);
                RobotVec[robotId] -> TypeArticleCarry = 0;
                BuySellFlag = 1;
            }
            if(RobotVec[robotId]->GetWorkBenchID() == RobotVec[robotId]->GetWantToCloseWBID() && RobotVec[robotId]->TypeArticleCarry == 0){
                if(WorkBenchVec[RobotVec[robotId]->GetWorkBenchID()]->ProductStatus == 1 ){
                    int tmp;
                    if(!All7(WorkBenchSelf[7], WorkBenchVec[RobotVec[robotId]->GetWorkBenchID()]->WorkBenchKind, &tmp, robotId)){
                        if(9000 - frameID < 350 && WorkBenchVec[RobotVec[robotId]->GetWorkBenchID()]->WorkBenchKind == 7||
                           9000 - frameID < 250 && WorkBenchVec[RobotVec[robotId]->GetWorkBenchID()]->WorkBenchKind == 6||
                           9000 - frameID < 250 && WorkBenchVec[RobotVec[robotId]->GetWorkBenchID()]->WorkBenchKind == 5||
                           9000 - frameID < 250 && WorkBenchVec[RobotVec[robotId]->GetWorkBenchID()]->WorkBenchKind == 4)
                        {}
                        else{
                            cout << "buy " << robotId << endl;
                            fflush(stdout);
                            BuySellFlag = 1;
                            int ID = RobotVec[robotId]->WorkBenchID;
                        }
                    }
                }
            }
            if(BuySellFlag == 1){
                if(RobotVec[robotId]->RobotMode == 0){
                    Cur[robotId] = (++Cur[robotId] % 4);
                    RobotVec[robotId]->WantTOCloseWBKind = Order[robotId][Cur[robotId]];
                }
                RobotVec[robotId] -> RobotMode = 0;
                vector<int>& Erase = (WorkBenchVec[RobotVec[robotId]->WorkBenchID] -> RobotScheduled);
                if(find(Erase.begin(), Erase.end(), robotId) != Erase.end())
                    Erase.erase(find(Erase.begin(), Erase.end(), robotId));
                RobotVec[robotId] -> HaveTarget = -1;
            }
        }
        printf("OK\n", frameID);
        fflush(stdout);
    }
    WorkBenchMemRelease(WorkBenchVec);
    RobotMemRelease(RobotVec);
    return 0;
}


// bool Buy456(Robot* _Robot, vector<Workbench*>& _WorkBench456){
//     if(_Robot -> TypeArticleCarry != 0){
//         return false;
//     }
//     if(_WorkBench456.empty()){
//         return false;
//     }
//     if(_Robot -> RobotMode != 0){
//         return false;
//     }
//     int MinID456 = FindMinDistanceTargetWB(_Robot, _WorkBench456);
//     // if(_WorkBench456[MinID456] -> RobotScheduled != -1)
//     //     return false;
//     _Robot -> WantToCloseWBID = _WorkBench456[MinID456] -> WorkBenchID;
//     _WorkBench456.erase(_WorkBench456.begin() + MinID456);
//     _Robot -> RobotMode = 1;
//     return true;
// }
bool Buy7(Robot* _Robot, vector<Workbench*>& _WorkBench7){
    if(_Robot -> TypeArticleCarry != 0){
        return false;
    }
    if(_WorkBench7.empty()){
        return false;
    }
    if(_Robot -> RobotMode != 0){
        return false;
    }
    int MinID456 = FindMinDistanceTargetWB(_Robot, _WorkBench7);
    if(!_WorkBench7[MinID456] -> RobotScheduled.empty())
        return false;
    _Robot -> WantToCloseWBID = _WorkBench7[MinID456] -> WorkBenchID;
    _WorkBench7.erase(_WorkBench7.begin() + MinID456);
    _Robot -> RobotMode = 3;
    return true;
}
void ShowRobotInfo(vector<Robot*>& _RobotVec){
    int i;
    cerr << "Robot Target" << endl;
    for(i = 0; i < 4; i++){
        cerr << RobotVec[i]->WantToCloseWBID << ' ';
    }
    cerr << endl;
    cerr << "Robot Mode" << endl;
    for(i = 0; i < 4; i++){
        cerr <<RobotVec[i]->RobotMode << ' ';
    }
    cerr << endl;
    cerr << "Robot HaveTarget"<< endl;
    for(i = 0; i < 4; i++){
        cerr << RobotVec[i] -> HaveTarget << ' '; 
    }
    cerr << endl;
    cerr << "Robot Find Status" << endl;
    for(i = 0; i < 4; i++){
        // _RobotVec[9]->FindStatus = 0;
        cerr << RobotVec[i] -> FindStatus << " ";
        RobotVec[i]->FindStatus = 0;
    }
    cerr << endl;
}

void FlushProduct(Robot* _Robot, vector<Workbench*>& _WorkBenchVec){
    if(_Robot -> RobotID != 3)
        return;
    if(_Robot->RobotMode == 0)
        return;
    if(_WorkBenchVec[_Robot -> WantToCloseWBID] -> ProductStatus == 0){
        _Robot->HaveTarget = -1;
        _Robot->RobotMode = 0;
    }
}

bool All(vector<Workbench*>& _WorkBenchVec, int Type, Robot* _Robot){
    int i;
    // cerr << "WorkBenchvec[0]kind" << _WorkBenchVec[0] -> WorkBenchKind << endl;
    if(_WorkBenchVec[0] -> WorkBenchKind == 1 || _WorkBenchVec[0] -> WorkBenchKind == 2 || _WorkBenchVec[0] -> WorkBenchKind == 3)
        return false;
    for(i = 0; i < _WorkBenchVec.size(); i++){
        if((_WorkBenchVec[i]-> MaterialStatus & (1 << Type)) == 0){
            // exit(1);
            return false;
        }
    }
    _Robot->DoOnce++;
    return true;
}

bool All7(vector<Workbench*>& _WorkBenchVec, int Type, int *CheckID, int RobotID){
    int i;
    if(_WorkBenchVec.empty())
        return false;
    // cerr << "WorkBenchvec[0]kind" << _WorkBenchVec[0] -> WorkBenchKind << endl;
    // if(_WorkBenchVec[0] -> WorkBenchKind == 1 || _WorkBenchVec[0] -> WorkBenchKind == 2 || _WorkBenchVec[0] -> WorkBenchKind == 3)    
    if(Type == 1 || Type == 2 || Type == 3 || Type == 7)
        return false;
    for(i = 0; i < _WorkBenchVec.size(); i++){
        if((_WorkBenchVec[i]-> MaterialStatus & (1 << Type)) == 0){//检查所有的7号工作台，如果有一个工作台对应456没满，则可以购买456
            if(_WorkBenchVec[i]->RobotScheduled.empty()){
                return false;
            }
            if(_WorkBenchVec[i]->CheckLock(RobotVec,Type,RobotID)){
                return true;
            }else{
                return false;
            }
        }
    }
    return true;
}

bool IsBuy(int Kind, vector<vector<Workbench*>>& _WorkBenchSellSelf){
    if(Kind == 1 || Kind == 2 || Kind == 3)
        return false;
    if(_WorkBenchSellSelf[Kind].empty())
        return false;
    return true;
}

double RobotCloseWorkBench(Robot* _Robot, vector<Workbench*>& _WorkBenchVec){
    const double Limit = 6.0;
    if(_Robot -> Axis[0] < Limit || _Robot -> Axis[0] > 50 - Limit || _Robot -> Axis[1] < Limit || _Robot -> Axis[1] > 50 - Limit){
        if(abs(_Robot->Axis - WorkBenchVec[_Robot->WantToCloseWBID]->Axis) < 0.4 && _Robot->TypeArticleCarry != 0){
            return -1.0;
        }
        return 1.0;
    }
    return NormalSpeed - 2.0;
}

double RobotCloseWall(Robot* _Robot){
    const double Limit = 6.0;
    if(_Robot -> Axis[0] < Limit || _Robot -> Axis[0] > 50 - Limit)
        return 4.0;
    if(_Robot -> Axis[1] < Limit || _Robot -> Axis[1] > 50 - Limit)
        return 4.0;
    return NormalSpeed;
}

void Showyuyue(Workbench* _WorkBench){
    cerr << "WorkBench : " << _WorkBench->WorkBenchID << " Kind : " << _WorkBench->ProductKind << endl;
    for(int tmp: _WorkBench->RobotScheduled){
        cerr << tmp << ' ';
    }
    cerr << endl;
    cerr << "WorkBenchyuyue"<< _WorkBench->WorkBenchID << " Kind : " << _WorkBench->ProductKind << endl;
    cerr << _WorkBench->MaterialStatus << endl;
    cerr << endl;
}

bool Robot2Robot(Robot* _Robot, vector<Robot*> _RobotVec){
    if(_Robot->AngleFrameIDFlag == 1)
        return false;
    map<double, int> Cpy;
    int i;
    double tmp = 1e10;
    for(i = 0; i < 4; i++){
        if(i != _Robot->RobotID){
            tmp = _Robot->Axis - _RobotVec[i]->Axis;
            Cpy.insert(pair<double,int>(tmp,i));
        }
        if(_Robot->TypeArticleCarry != 0 || _RobotVec[i] -> TypeArticleCarry != 0){
            if(tmp < 3 && _Robot->TypeArticleCarry < _RobotVec[i] -> TypeArticleCarry){
                return true;
            }
        }
    }
    return false;
}

void Buy456(Robot* _Robot, vector<Workbench*>& _WorkBench456){
    if(_Robot->RobotID != 3)
        return;
    if(_Robot->RobotMode != 0)
        return;
    if(_WorkBench456.empty())
        return;
    int MinID456 = FindMinDistanceTargetWB(_Robot, _WorkBench456);
    _WorkBench456.erase(_WorkBench456.begin() + MinID456);
    _Robot->RobotMode  = 1;
    _Robot->WantToCloseWBID = _WorkBench456[MinID456]->WorkBenchID;
    timeswap++;
}

bool Loss7(Workbench * _WorkBench, int Type){
    int i;
    for(i = 0; i < WorkBenchSelf[7].size(); i++){
        if(_WorkBench->MaterialStatus & (1 << Type) == 0){
            return true;
        } 
    }
    return false;
}