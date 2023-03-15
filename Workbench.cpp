#include "Workbench.h"
void WorkBenchAndRobotRead(const string& _InitString, vector<Workbench*>& _WorkBenchVec, vector<Robot*>& _RobotVec, int yaxis){
    int i;
    Workbench* WorkBenchPtr = nullptr;
    Robot* RobotPtr = nullptr;
    for(i = 0; i < _InitString.size() - 1; i++){
        char get = _InitString[i];//get = map[yaxis][i]
        vector<double> axis;
        axis.push_back(0.25 + 0.5 * i);//x
        axis.push_back(49.75 - 0.5 * yaxis);//y
        // cerr << get << endl;
        if(get != '.' && get != 'A'){
            WorkBenchPtr = new Workbench(get, axis, IDCount++);
            // WorkBenchPtr = new Workbench(get, vector<double>(2, 0.0), IDCount++);
            WorkBenchPtr->ShowWorkBench();
            _WorkBenchVec.push_back(WorkBenchPtr);
        }
        if(get == 'A'){
            RobotPtr = new Robot();
            // axis[0] = 0.0;
            // axis[1] = 0.0;
            RobotPtr->GetAxis() = axis;
            RobotPtr->RobotID = RobotIDCount++;
            _RobotVec.push_back(RobotPtr);
        }
    }
}

void WorkBenchAllocate(const vector<Workbench*>& _WorkBenchVec, vector<vector<Workbench*>> &_WorkBenchSelf){
    int i;
    for(i = 0; i < _WorkBenchVec.size(); i++){
        int id = _WorkBenchVec[i]->GetWorkBenchKind();
        _WorkBenchSelf[id].push_back(_WorkBenchVec[i]);
    }
}

void WorkBenchMemRelease(const vector<Workbench*>& _WorkBenchVec){
    int i;
    for(i = 0; i < _WorkBenchVec.size(); i++){
        delete _WorkBenchVec[i];
    }
} 
void dec2bin(int _dec, vector<int>& _store){
    while(_dec > 0){
        if(_dec & 1)
            _store.push_back(1);
        else if((_dec & 1) == 0)
            _store.push_back(0);
        _dec >>= 1;
    }
}
void Workbench::ShowWorkBench()const{
    vector<int>ShowBinary;
    int dec = MaterialStatus;
    dec2bin(dec, ShowBinary);
    reverse(ShowBinary.begin(), ShowBinary.end());
    cerr << "WorkBenchID : " << setw(2) << WorkBenchID << " WorkBenchKind : " <<  WorkBenchKind 
         << " axisx : " << setw(5) << Axis[0] << " axisy : "  << setw(5) << Axis[1]  << " ProductStatus : " << ProductStatus << " MaterialStatus : ";
    if(ShowBinary.empty())
        cerr << setw(9) << setfill('0') << 0;
    else
        for_each(ShowBinary.begin(), ShowBinary.end(),[](int x){cerr << x;}); 
    cerr << endl;
}

bool Workbench::HaveLock(int CarryType, vector<Robot*>& _Robot){
    // cerr << "RobotScheduled : " << RobotScheduled << endl;
    if(CarryType == 0){
        if(RobotScheduled == -1)
            return false;
        else   
            return true;
    }else{
        // cerr << "WorBenchID" << WorkBenchID << endl;
        // exit(1);
        if(MaterialStatus & (1 << CarryType)){//如果工作台有了对应材料，锁了
            return true;
        }else if(RobotScheduled != -1 && _Robot[RobotScheduled]->TypeArticleCarry == CarryType){//预约工作台的机器人带的材料和输入的材料一致， 锁了
            // exit(1);
            return true;
        }else{
            return false;
        }
    }
}

Workbench::Workbench(char _WorkBenchKind, const vector<double>& _axis, int _WorkBenchID){
    if(_WorkBenchKind == '1'){
        WorkBenchKind = 1;
        Role = 0;
        RestWorkTime = WORKTIME1;
        ProductKind = 1;
        BuyingPrice = BUYINGPRICE1;
        SellingPrice = SELLINGPRICE1; 
    }else if(_WorkBenchKind == '2'){
        WorkBenchKind = 2;
        Role = 0;
        RestWorkTime = WORKTIME2;
        ProductKind = 2;
        BuyingPrice = BUYINGPRICE2;
        SellingPrice = SELLINGPRICE2; 
    }else if(_WorkBenchKind == '3'){
        WorkBenchKind = 3;
        Role = 0;
        RestWorkTime = WORKTIME3;
        ProductKind = 3;
        BuyingPrice = BUYINGPRICE3;
        SellingPrice = SELLINGPRICE3; 
    }else if(_WorkBenchKind == '4'){
        WorkBenchKind = 4;
        Role = 1;
        Material.push_back(1);
        Material.push_back(2);
        RestWorkTime = WORKTIME4;
        ProductKind = 4;
        BuyingPrice = BUYINGPRICE4;
        SellingPrice = SELLINGPRICE4; 
    }else if(_WorkBenchKind == '5'){
        WorkBenchKind = 5;
        Role = 1;
        Material.push_back(1);
        Material.push_back(3);
        RestWorkTime = WORKTIME5;
        ProductKind = 5;
        BuyingPrice = BUYINGPRICE5;
        SellingPrice = SELLINGPRICE5; 
    }else if(_WorkBenchKind == '6'){
        WorkBenchKind = 6;
        Role = 1;
        Material.push_back(2);
        Material.push_back(3);
        RestWorkTime = WORKTIME6;
        ProductKind = 6;
        BuyingPrice = BUYINGPRICE6;
        SellingPrice = SELLINGPRICE6; 
    }else if(_WorkBenchKind == '7'){
        WorkBenchKind = 7;
        Role = 1;
        Material.push_back(4);
        Material.push_back(5);
        Material.push_back(6);
        RestWorkTime = WORKTIME7;
        ProductKind = 7;
        BuyingPrice = BUYINGPRICE7;
        SellingPrice = SELLINGPRICE7; 
    }else if(_WorkBenchKind == '8'){
        WorkBenchKind = 8;
        Role = 2;
        Material.push_back(7);
        RestWorkTime = WORKTIME8;
        ProductKind = 8;
        BuyingPrice = BUYINGPRICE8;
        SellingPrice = SELLINGPRICE8; 
    }else if(_WorkBenchKind == '9'){
        WorkBenchKind = 9;
        Role = 2;
        for(int i = 1; i < 8; i++){
            Material.push_back(i);
        }
        RestWorkTime = WORKTIME9;
        ProductKind = 9;
        BuyingPrice = BUYINGPRICE9;
        SellingPrice = SELLINGPRICE9; 
    }else{
        cerr << "The wrong workbench type was entered : " << (int)_WorkBenchKind <<  endl;
        exit(1);
    }
    Axis = _axis;
    WorkBenchID = _WorkBenchID;
}