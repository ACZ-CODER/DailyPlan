#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <regex>
#include <filesystem>

// 定义计划结构体
struct Plan{
    std::string date;
    std::string content;
    int priority;
    bool completed;
};

std::vector<Plan> plans;

//获取exe文件路径
std::filesystem::path getExeDir(){
    char path[MAX_PATH];
    GetModuleFileNameA(NULL,path,MAX_PATH);
    return std::filesystem::path(path).parent_path();
}

// 从文件中获取计划
void loadPlansFromFile(const std::filesystem::path& csvPathName) {
    std::ifstream file(csvPathName);
    if (!file) return;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        Plan p;
        std::string priorityStr, completedStr;
        std::getline(ss, p.date, ',');
        std::getline(ss, p.content, ',');
        std::getline(ss, priorityStr, ',');
        std::getline(ss, completedStr, ',');
        p.priority = std::stoi(priorityStr);
        p.completed = (completedStr == "1");
        plans.push_back(p);
    }
}
// 自动保存计划
void savePlansToFile(const std::filesystem::path& csvPathName) {
    std::ofstream file(csvPathName);
    for (auto& p : plans) {
        file << p.date << ","
             << p.content << ","
             << p.priority << ","
             << (p.completed ? "1" : "0") << "\n";
    }
}

// 定义计划添加函数
void addPlan(){
    Plan plan;

    std::regex datePattern(R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|1[0-9]|[12]\d|3[01])$)");
    std::regex priorityPattern(R"(^[1-3]$)");

    do{
        std::cout << "输入日期(YYYY-MM-DD): ";
        std::cin >> plan.date;
        std::cin.ignore();
        if(!std::regex_match(plan.date,datePattern)){
            std::cout << "重新输入正确的日期！ \n";
        }
    }while(!std::regex_match(plan.date,datePattern));

    std::cout << "输入任务内容：";
    std::getline(std::cin,plan.content);
    do{
        std::cout << "输入优先级 (1=高 2=中 3=低): ";
        std::cin >> plan.priority;
        if(!std::regex_match(std::to_string(plan.priority),priorityPattern)){
            std::cout << "重新输入优先级! \n";
        }
    }while(!std::regex_match(std::to_string(plan.priority),priorityPattern));
    
    plan.completed = false;
    plans.push_back(plan);
}
// 展示全部计划
void showPlans(){
    std::cout << "\n全部计划列表\n";
    for (size_t i = 0; i < plans.size(); i++){
        std::cout << i+1 << ". [" << (plans[i].completed ? "完成" : "未完成") << "]"
            << plans[i].date << " - " << plans[i].content << "-(优先级" << plans[i].priority << ")\n";
    }
}
// 展示未完成计划
void showIncompletePlans(){
    std::cout << "\n未完成计划列表\n";
    int j = 0;
    for (size_t i = 0; i < plans.size(); i++){
        if(!plans[i].completed){
            std::cout << j+1 << ". [未完成]" << plans[i].date << " - " << plans[i].content << "-(优先级" << plans[i].priority << ")\n";
            j++;
        }
    }
}
// 完成计划中一项
void markComplete(){
    showIncompletePlans();
    int index;
    std::cout << "输入要标记完成的计划序号: ";
    std::cin >> index;
    int j = 0;
    // 确保 index 在有效范围
    int unfinishedCount = 0;
    for (size_t i = 0; i < plans.size(); i++) {
        if (!plans[i].completed) unfinishedCount++;
    }
    if (index <= 0 || index > unfinishedCount) {
        std::cout << "无效选择。\n";
        return;
    }
    for (size_t i = 0; i < plans.size(); i++){
        if(!plans[i].completed){
            j++;
            if(j == index){
                plans[i].completed = true;
                std::cout << "已标记完成！\n";
                break;
            }
        }
    }
}
// 删除计划某一项
void deletePlan(){
    showPlans();
    int index;
    std::cout << "输入要删除的计划序号: ";
    std::cin >> index;
    if (index > 0 && index <= plans.size()) {
        plans.erase(plans.begin() + index - 1);
    }else{
        std::cout << "无效选择。\n";
        return;
    }
}

void showMenu() {
    std::cout << "\n=== 每日计划管理 ===\n";
    std::cout << "1. 查看计划\n";
    std::cout << "2. 添加计划\n";
    std::cout << "3. 未做计划\n";
    std::cout << "4. 标记完成\n";
    std::cout << "5. 删除计划\n";
    std::cout << "0. 退出\n";
    std::cout << "请选择: ";
}

int main() {
    std::filesystem::path csvPathName = getExeDir()/"plans.csv";
    loadPlansFromFile(csvPathName);
    
    // 将 Windows 控制台改成 UTF-8
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    int choice;
    do {
        showMenu();
        std::cin >> choice;
        switch (choice) {
            case 1: showPlans(); break;
            case 2: addPlan(); break;
            case 3: showIncompletePlans(); break;
            case 4: markComplete(); break;
            case 5: deletePlan(); break;
            case 0: savePlansToFile(csvPathName); break;
            default: std::cout << "无效选择\n";
        }
    } while (choice != 0);

    return 0;
}