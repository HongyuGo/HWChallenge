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
const double NormalSpeed = 6.0;
vector<double> RobotSetLineSpeed = {NormalSpeed-0.4,NormalSpeed-0.2,NormalSpeed,NormalSpeed-0.5};
vector<double> RobotSetAngleSpeed = {2.9,2.8,3.0,2.7};

// char map[MAPLEN][MAPLEN];
double CalculateAngleSpeed(Robot* _Robot, vector<Workbench*>& _WorkBenchVec, int MinID);
bool Buy456(Robot* _Robot, vector<Workbench*>& _WorkBench456);
bool Buy7(Robot* _Robot, vector<Workbench*>& _WorkBench7);
void ShowRobotInfo(vector<Robot*>& _RobotVec);
void FlushProduct(Robot* _Robot, vector<Workbench*>& _WorkBenchVec);
bool All(vector<Workbench*>& _WorkBenchVec, int Type);
bool All7(vector<Workbench*>& _WorkBenchVec, int Type, int* CheckID, int RobotID);
bool IsBuy(int Kind, vector<vector<Workbench*>>& _WorkBenchSellSelf);
double RobotCloseWorkBench(Robot* _Robot, vector<Workbench*>& _WorkBenchVec);
double RobotCloseWall(Robot* _Robot);
void Showyuyue(Workbench* _WorkBench);
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
    int MinID = _WorkBenchVec.size() + 1;
    vector<double> _axis = _Robot->GetAxis();
    double MinDistance = 1e10;
    for(i = 0; i < _WorkBenchVec.size(); i++){
        double DistanceTmp = _axis - _WorkBenchVec[i]->GetAxis();
        if(DistanceTmp < MinDistance && !(_WorkBenchVec[i]->HaveLock(_Robot->TypeArticleCarry, RobotVec, _Robot -> RobotID))){
            MinDistance = DistanceTmp;
            MinID= i;
        }
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
            // if(WorkBenchVec[i] -> ProductStatus == 1 && WorkBenchVec[i] -> WorkBenchKind == 6 && WorkBenchVec[i] -> RobotScheduled == -1 && !All7(WorkBenchSelf[7], 6, &CheckID)){
            //     if(CheckID == i){
            //     count6++;
            //     WorkBench6.push_back(WorkBenchVec[i]);
            //     WorkBench456.push_back(WorkBenchVec[i]);
            //     }
            // }
            // if(WorkBenchVec[i] -> ProductStatus == 1 && WorkBenchVec[i] -> WorkBenchKind == 5 && WorkBenchVec[i] -> RobotScheduled == -1 && !All7(WorkBenchSelf[7], 5, &CheckID)){
            //     if(CheckID == i){
            //     count5++;
            //     WorkBench5.push_back(WorkBenchVec[i]);
            //     WorkBench456.push_back(WorkBenchVec[i]);
            //     }
            // }
            // if(WorkBenchVec[i] -> ProductStatus == 1 && WorkBenchVec[i] -> WorkBenchKind == 4 && WorkBenchVec[i] -> RobotScheduled == -1 && !All7(WorkBenchSelf[7], 4, &CheckID)){
            //     if(CheckID == i){
            //     count4++;
            //     WorkBench4.push_back(WorkBenchVec[i]);
            //     WorkBench456.push_back(WorkBenchVec[i]);
            //     }
            // }
        }
        WorkBenchSellSelf[4] = WorkBench4;
        WorkBenchSellSelf[5] =WorkBench5;
        WorkBenchSellSelf[6] = WorkBench6;
        int C = 0;
        if(count4 <= count5){
            if(count4 <= count6)
                C = 4;
            else    
                C = 6;
        }
        else{
            if(count5 <= count6)
                C = 5;
            else
                C = 6;
        }
        Order[3] = Order[C - 4];
        Order[3] = Order[C - 4];
        
        for(i = 0; i < 4; i++){
            if(RobotVec[i] -> RobotMode == 3){
                Cur[i] = 0;
                AngleSpeed[i] = Move(RobotVec[i], RobotVec[i]->WantToCloseWBID, WorkBenchVec);              
                continue;
            }
            FlushProduct(RobotVec[i], WorkBenchVec);
            if(RobotVec[i] -> TypeArticleCarry == 4 || RobotVec[i] -> TypeArticleCarry == 5 || RobotVec[i] -> TypeArticleCarry == 6)
                RobotVec[i] -> RobotMode = 2;
            if(RobotVec[i] -> TypeArticleCarry == 7){
                RobotVec[i] ->RobotMode = 4;
            }
            if(RobotVec[i] -> RobotMode == 0){
                RobotVec[i]->WantTOCloseWBKind = Order[i][Cur[i]];
                if(All(WorkBenchSelf[Order[i][1]], RobotVec[i]->WantTOCloseWBKind)){
                    swap(Order[i][0], Order[i][2]);
                    RobotVec[i]->WantTOCloseWBKind = Order[i][Cur[i]];
                }
                AngleSpeed[i] =  CalculateDistance(RobotVec[i], WorkBenchSelf[RobotVec[i]->WantTOCloseWBKind]);
            }else if(RobotVec[i] -> RobotMode == 1){
                Cur[i] = 0;
                AngleSpeed[i] = Move(RobotVec[i], RobotVec[i]->WantToCloseWBID, WorkBenchVec);
            }
            if(RobotVec[i] -> RobotMode == 2){
                if(WorkBenchSelf[7].size() !=0){
                    Cur[i] = 0;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[7]);
                    
                }
                else if(WorkBenchSelf[9].size() != 0){
                    Cur[i] = 0;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[9]);
                }
            }
            if(RobotVec[i] -> RobotMode == 4){
                if(WorkBenchSelf[8].size() != 0){
                    Cur[i] = 0;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[8]);
                }
                else if(WorkBenchSelf[9].size() != 0){
                    Cur[i] = 0;
                    AngleSpeed[i] = CalculateDistance(RobotVec[i], WorkBenchSelf[9]);
                }
            }
        }

        // ShowRobotInfo(RobotVec);
        // Showyuyue(WorkBenchVec[16]);
        // Showyuyue(WorkBenchVec[14]);
        // Showyuyue(WorkBenchVec[0]);
        for(i = 0; i < 4; i++){
            if(RobotVec[i] -> RobotMode == 3)
                exit(1);
        }
        printf("%d\n", frameID);
        for(i = 0; i < 4; i++){
            if(abs(AngleSpeed[i]) < 0.2){
                LineSpeed[i] = RobotSetLineSpeed[i];
                LineSpeed[i] = RobotCloseWall(RobotVec[i]);
            }
            else
                LineSpeed[i] = RobotCloseWorkBench(RobotVec[i], WorkBenchVec);
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
    // if(_WorkBench456[MinID456] -> RobotScheduled != -1)
    //     return false;
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
    if(_Robot->RobotMode == 0)
        return;
        
    // if(_WorkBenchVec[_Robot -> WantToCloseWBID] -> ProductStatus == 0){
    //     _Robot->HaveTarget = -1;
    //     _Robot->RobotMode = 0;
    // }
}

bool All(vector<Workbench*>& _WorkBenchVec, int Type){
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
    if(abs(_Robot->Axis - WorkBenchVec[_Robot->WantToCloseWBID]->Axis) < 0.4 && _Robot->TypeArticleCarry != 0){
        return -1.0;
    }
    return 1.0;
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


void ControlLineSpeed(){

}