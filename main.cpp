#include "Robot.h"
#include "Workbench.h"
#include <cstring>
#include "Params.h"
#define MAPLEN (100)
string map[MAPLEN];
vector<int> WorkBenchNum(10,0);
vector<Workbench*>WorkBenchVec;
vector<vector<Workbench*>> WorkBenchSelf(10);
vector<Robot*>RobotVec;
int Money;

// char map[MAPLEN][MAPLEN];
bool Buy456(Robot* _Robot, vector<Workbench*>& _WorkBench456);
bool Buy7(Robot* _Robot, vector<Workbench*>& _WorkBench7);
void ShowRobotInfo(vector<Robot*>& _RobotVec);
void FlushProduct(Robot* _Robot, vector<Workbench*> _WorkBenchVec);
bool All(vector<Workbench*> _WorkBenchVec, int Type);
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
            return true;
        }
        map[i] = line;
        // cerr << map[i] << endl;
        // exit(1);
        WorkBenchAndRobotRead(map[i], WorkBenchVec, RobotVec, i); 
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
int FindMinDistanceTargetWB(Robot* _Robot, vector<Workbench*>& _WorkBenchVec){
    int i;
    int MinID = _WorkBenchVec.size() - 1;
    vector<double> _axis = _Robot->GetAxis();
    double MinDistance = 1e10;
    for(i = 0; i < _WorkBenchVec.size(); i++){
        double DistanceTmp = _axis - _WorkBenchVec[i]->GetAxis();
        // cerr << _WorkBenchVec[i] -> RobotScheduled << endl;
        // exit(1);
        if(DistanceTmp < MinDistance && !(_WorkBenchVec[i]->HaveLock(_Robot->TypeArticleCarry, RobotVec))){
        // if(DistanceTmp < MinDistance && _WorkBenchVec[i] -> RobotScheduled == 0){
            // cerr << "Min test" << endl;
            MinDistance = DistanceTmp;
            MinID= i;
        }
    }
    // cerr << "MinID" << MinID << endl;
    // exit(1);
    if(_WorkBenchVec[MinID] -> RobotScheduled == -1)
        _WorkBenchVec[MinID] -> RobotScheduled = _Robot->RobotID;
    cerr << "MinID " << MinID << endl;
    // cerr << "TargetWB " << _WorkBenchVec[MinID]->WorkBenchID << endl;
    return MinID;
}
double CalculateDistance(Robot* _Robot, vector<Workbench*>& _WorkBenchVec){
    int i;
    int MinID;
    vector<double> _axis = _Robot->GetAxis();
    if(_Robot->HaveTarget == -1){
        MinID = FindMinDistanceTargetWB(_Robot, _WorkBenchVec);
        _Robot->HaveTarget = MinID;
    }
    else    
        MinID = _Robot->HaveTarget;
    // cerr << "MinID" << MinID << endl;
    // cerr << "teststetstes" << endl;
        // _Robot->HaveTargetWBFlag = 1;
    // }
    // cerr << "MInID : " << MinID << endl;
    int flag = 0;
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
    // cerr << "Angle" << Angle << endl;
    double AngleDifference = abs(Angle - _Robot->GetTowards());
    if(abs(AngleDifference) < 0.5)
        flag = Angle - _Robot->GetTowards() > 0.0 ? 1 : -1; // 1:ni -1 sun;
    else    
        flag = 1;
    double RealDis =  sqrt(XDistance * XDistance + YDistance * YDistance);
    _Robot->GetWantToCloseWBID() = _WorkBenchVec[MinID]->GetWorkBenchID();
    _Robot->GetRobotWorkBenchDis() = RealDis;
    _Robot->GetAngleDifference() = AngleDifference;
    double AngleSpeed = AngleDifference > 1.5 ? 1.5 : AngleDifference;
    // cerr << "AngleSpeed" << AngleSpeed << endl;
    // cerr << "enenene " << endl;
    return AngleSpeed * flag;
}
double Move(Robot* _Robot, int _WorkBenchID, vector<Workbench*>& _WorkBenchVec){
    vector<double> _axis = _Robot->GetAxis();
    int flag = 0;
    double XDistance =  _WorkBenchVec[_WorkBenchID]->GetAxis()[0] - _axis[0];
    double YDistance =  _WorkBenchVec[_WorkBenchID]->GetAxis()[1] - _axis[1];
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
    // cerr << "Angle" << Angle << endl;
    double AngleDifference = abs(Angle - _Robot->GetTowards());
    if(abs(AngleDifference) < 0.5)
        flag = Angle - _Robot->GetTowards() > 0.0 ? 1 : -1; // 1:ni -1 sun;
    else    
        flag = 1;
    double RealDis =  sqrt(XDistance * XDistance + YDistance * YDistance);
    _Robot->GetWantToCloseWBID() = _WorkBenchVec[_WorkBenchID]->GetWorkBenchID();
    _Robot->GetRobotWorkBenchDis() = RealDis;
    _Robot->GetAngleDifference() = AngleDifference;
    double AngleSpeed = AngleDifference > 1.5 ? 1.5 : AngleDifference;
    // cerr << "AngleSpeed" << AngleSpeed << endl;
    // cerr << "enenene " << endl;
    return AngleSpeed * flag;
}

int main() {
    // vector<double> ve1(2,1.0);
    // vector<double> ve2(2,2.0);
    // cout << ve1 - ve2 <<endl;
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
    vector<int> Order3 = {2, 6, 3, 6};



    vector<int> Cur(4,0);
    vector<double> AngleSpeed(4, 0.0);
    vector<vector<int>>Order;
    Order.push_back(Order0);
    Order.push_back(Order1);
    Order.push_back(Order2);
    Order.push_back(Order3);
    vector<double> LineSpeed(4,0.0);
    while (scanf("%d", &frameID) != EOF) {
        cin >> Money;
        cin >> WorkBenchNum;
        // cerr << "frameID " << frameID << " Money " << Money <<" WorkBenchNum " << WorkBenchNum << endl;
        ReadInfo(WorkBenchNum, WorkBenchVec, RobotVec);
        vector<Workbench*> WorkBench456;
        vector<Workbench*> WorkBench7;
        for(i = 0; i < WorkBenchNum; i++){
            if(WorkBenchVec[i] -> ProductStatus == 1 && (WorkBenchVec[i] -> WorkBenchKind == 4 || WorkBenchVec[i] -> WorkBenchKind == 5 || WorkBenchVec[i] -> WorkBenchKind == 6 )){
                WorkBench456.push_back(WorkBenchVec[i]);
            }
            if(WorkBenchVec[i] -> ProductStatus == 1 && WorkBenchVec[i] -> WorkBenchKind == 7){
                WorkBench7.push_back(WorkBenchVec[i]);
            }
        }
        cerr << "WorkBench7 : " <<  WorkBench7.size() << endl;
        for(i = 0; i < 4; i++){
            Buy7(RobotVec[i],WorkBench7);
            if(RobotVec[i] -> RobotMode == 3){
                Cur[i] = 0;
                AngleSpeed[i] = Move(RobotVec[i], RobotVec[i]->WantToCloseWBID, WorkBenchVec);              
                continue;
            }
            Buy456(RobotVec[i], WorkBench456);
            FlushProduct(RobotVec[i], WorkBenchVec);
            if(RobotVec[i] -> TypeArticleCarry == 4 || RobotVec[i] -> TypeArticleCarry == 5 || RobotVec[i] -> TypeArticleCarry == 6)
                RobotVec[i] -> RobotMode = 2;
            if(RobotVec[i] -> TypeArticleCarry == 7){
                RobotVec[i] ->RobotMode = 4;
            }
            if(RobotVec[i] -> RobotMode == 0){
                // start = 0;
                // if(All(WorkBenchSelf[Order], 2)){
                //     swap(Order[i][0], Order[i][2]);
                //     start = 2;
                // }
                // Cur[i] = (Cur[i] + start) % 4;
                RobotVec[i]->WantTOCloseWBKind = Order[i][Cur[i]];
                AngleSpeed[i] =  CalculateDistance(RobotVec[i], WorkBenchSelf[RobotVec[i]->WantTOCloseWBKind]);
            }else if(RobotVec[i] -> RobotMode == 1){
                Cur[i] = 0;
                AngleSpeed[i] = Move(RobotVec[i], RobotVec[i]->WantToCloseWBID, WorkBenchVec);
            }
            if(RobotVec[i] -> RobotMode == 2){
                if(WorkBenchSelf[7].size() !=0){
                    Cur[i] = 0;
                    // cerr << "7 : " << WorkBenchSelf[7].size() << endl;
                    // cerr << "have target" << RobotVec[i]->HaveTarget << endl;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[7]);
                }
                if(WorkBenchSelf[9].size() != 0){
                    Cur[i] = 0;
                    // cerr << "9 : " << WorkBenchSelf[7].size() << endl;
                    // cerr << "have target" << RobotVec[i]->HaveTarget << endl;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[9]);
                }
            }
            if(RobotVec[i] -> RobotMode == 4){
                if(WorkBenchSelf[8].size() != 0){
                    Cur[i] = 0;
                    // cerr << "9 : " << WorkBenchSelf[7].size() << endl;
                    // cerr << "have target" << RobotVec[i]->HaveTarget << endl;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[8]);
                }
                if(WorkBenchSelf[9].size() != 0){
                    Cur[i] = 0;
                    // cerr << "9 : " << WorkBenchSelf[7].size() << endl;
                    // cerr << "have target" << RobotVec[i]->HaveTarget << endl;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[9]);
                }
            }
        }

        ShowRobotInfo(RobotVec);
        // cerr << "WorkBench22 " << WorkBenchVec[22] -> MaterialStatus << endl;
        // cerr << "WorkBench6" << WorkBenchSelf[6][1]->MaterialStatus << endl;
        printf("%d\n", frameID);
        for(i = 0; i < 4; i++){
            if(abs(AngleSpeed[i]) < 0.2)
                LineSpeed[i] = 3;
            else
                LineSpeed[i] = 0.5;
        }
        // cerr << "Speed" <<LineSpeed[0]<< endl;
        for(int robotId = 0; robotId < 4; robotId++){
            printf("forward %d %lf\n", robotId, LineSpeed[robotId]);
            // cerr << "robotId " << robotId << " Linespeed : " <<   LineSpeed[robotId] << endl;
            printf("rotate %d %f\n", robotId, AngleSpeed[robotId]);
            // cerr << "robotId " << robotId << " AngleSpeed : " <<  AngleSpeed[robotId] << endl;
            int BuySellFlag = 0;
            if(RobotVec[robotId]->GetWorkBenchID() == RobotVec[robotId]->GetWantToCloseWBID() && RobotVec[robotId]->TypeArticleCarry != 0){
                cout << "sell " << robotId << endl;
                fflush(stdout);
                RobotVec[robotId] -> TypeArticleCarry = 0;
                // Cur[robotId] = (++Cur[robotId] % 4);
                // RobotVec[robotId]->WantTOCloseWBKind = Order[robotId][Cur[robotId]];
                // WorkBenchVec[RobotVec[robotId]->WorkBenchID] -> RobotScheduled = -1;
                // RobotVec[robotId] -> HaveTarget = -1;
                BuySellFlag = 1;
            }
            if(robotId == 2){
                cerr << RobotVec[robotId]->GetWantToCloseWBID() << " " << RobotVec[robotId]->GetWorkBenchID() << endl;
            }
            if(RobotVec[robotId]->GetWorkBenchID() == RobotVec[robotId]->GetWantToCloseWBID() && RobotVec[robotId]->TypeArticleCarry == 0){
                if(WorkBenchVec[RobotVec[robotId]->GetWorkBenchID()]->ProductStatus == 1){
                    cout << "buy " << robotId << endl;
                    fflush(stdout);
                    BuySellFlag = 1;
                    int ID = RobotVec[robotId]->WorkBenchID;
                    // if(find_if(WorkBench456.begin(), WorkBench456.end(),[ID](Workbench* work){return work->WorkBenchID == ID;}) != WorkBench456.end()){
                    //     WorkBench456.erase(WorkBench456.begin() + ID);
                    // }
                }
                // Cur[robotId] = (++Cur[robotId] % 4);
                // RobotVec[robotId]->WantTOCloseWBKind = Order[robotId][Cur[robotId]];
                // WorkBenchVec[RobotVec[robotId]->WorkBenchID] -> RobotScheduled = -1;
                // RobotVec[robotId] -> HaveTarget = -1;
            }
            if(BuySellFlag == 1){
                if(RobotVec[robotId]->RobotMode == 0){
                    Cur[robotId] = (++Cur[robotId] % 4);
                    RobotVec[robotId]->WantTOCloseWBKind = Order[robotId][Cur[robotId]];
                }
                RobotVec[robotId] -> RobotMode = 0;
                WorkBenchVec[RobotVec[robotId]->WorkBenchID] -> RobotScheduled = -1;
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


bool Buy456(Robot* _Robot, vector<Workbench*>& _WorkBench456){
    if(_Robot -> TypeArticleCarry != 0){
        return false;
    }
    if(_WorkBench456.empty()){
        return false;
    }
    if(_Robot -> RobotMode != 0){
        return false;
    }
    int MinID456 = FindMinDistanceTargetWB(_Robot, _WorkBench456);
    if(_WorkBench456[MinID456] -> RobotScheduled != -1)
        return false;
    _Robot -> WantToCloseWBID = _WorkBench456[MinID456] -> WorkBenchID;
    _WorkBench456.erase(_WorkBench456.begin() + MinID456);
    _Robot -> RobotMode = 1;
    return true;
}
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
    if(_WorkBench7[MinID456] -> RobotScheduled != -1)
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
}

void FlushProduct(Robot* _Robot, vector<Workbench*> _WorkBenchVec){
    if(_Robot->RobotMode == 0)
        return;
    if(_WorkBenchVec[_Robot -> WantToCloseWBID] -> ProductStatus == 0){
        _Robot->HaveTarget = -1;
        _Robot->RobotMode = 0;
    }
}

bool All(vector<Workbench*> _WorkBenchVec, int Type){
    int i;
    for(i = 0; i < _WorkBenchVec.size(); i++){
        cerr << "---------------" << _WorkBenchVec[i]-> MaterialStatus <<endl;
        if((_WorkBenchVec[i]-> MaterialStatus & (1 << Type)) == 0){
            // exit(1);
            return false;
        }
    }
    return true;
}