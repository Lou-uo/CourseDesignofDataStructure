#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>		// 控制台颜色、清屏等系统操作
#include <conio.h>			// 按键输入（如_getch()）
#include <iomanip>
#include <cstdlib>			// 系统命令（如system()）
#include <limits>			// 输入流清空（numeric_limits）
#include <unordered_set>
#include <unordered_map>

using namespace std;

// 时间区间结构体：存储上课的信息
struct TimeSlot {
	string weekday;    // 星期
	string startTime;  // 开始时间
	string endTime;    // 结束时间
	string location;   // 教学地点
	
	TimeSlot(string weekday, string startTime, string endTime, string location)
	: weekday(weekday), startTime(startTime), endTime(endTime), location(location) {}
	
	// 判断两个时间区间是否冲突
	bool isConflict(const TimeSlot& other) const {
		return weekday == other.weekday && !(endTime <= other.startTime || startTime >= other.endTime);
	}
};

// 课程教学班：每个课程下的不同老师
struct CourseClass {
	string classId;     // 教学班编号
	string teacher;     // 授课教师
	TimeSlot timeSlot;  // 上课时间
	bool selected;      // 是否被选中
	
	CourseClass(string classId, string teacher, TimeSlot timeSlot)
	: classId(classId), teacher(teacher), timeSlot(timeSlot), selected(false) {}
};

// 课程类
class Course {
public:
	string id;                 // 课程编号
	string name;               // 课程名称
	int credit;                // 学分
	int semester;              // 学期
	vector<Course*> prerequisites;  // 前置课程
	vector<CourseClass> classes;    // 该课程下的所有教学班
	
	Course(string id, string name, int credit, int semester)
	: id(id), name(name), credit(credit), semester(semester) {}
	
	// 添加前置课程
	void addPrerequisite(Course* prereq) {
		prerequisites.push_back(prereq);
	}
	
	// 添加教学班
	void addCourseClass(const CourseClass& courseClass) {
		classes.push_back(courseClass);
	}
	
	// 检查该课程是否有已选中的教学班
	bool hasSelectedClass() const {
		for (const auto& cls : classes) {
			if (cls.selected) return true;
		}
		return false;
	}
	
	// 获取已选中的教学班
	const CourseClass* getSelectedClass() const {
		for (const auto& cls : classes) {
			if (cls.selected) return &cls;
		}
		return nullptr;
	}
	
	// 检查前置课程是否满足
	bool canSelect(const vector<Course*>& allCourses) const {
		for (Course* p : prerequisites) {
			if (!p->hasSelectedClass()) {
				return false;
			}
		}
		return true;
	}
};

// 选课系统类
class CourseSystem {
private:
	vector<Course*> allCourses;				// 存储所有的课程
	int totalCredits;						// 已选的总学分
	const int CREDIT_THRESHOLD = 20;		// 学分要求，我写的是20，可以修改的。
	
	// 递归查找所有依赖该课程的已选课程
	void findDependentCourses(Course* target, vector<Course*>& dependents) {
		for (Course* course : allCourses) {
			if (course->hasSelectedClass() && 				// 1、该课程已选
				find(course->prerequisites.begin(), 
					course->prerequisites.end(), target) 
				!= course->prerequisites.end()) {			// 2、前置包含该课程
				if (find(dependents.begin(), dependents.end(), course) == dependents.end()) {	// 3、避免重复添加
					dependents.push_back(course);
					findDependentCourses(course, dependents);
				}
			}
		}
	}
	
	// 检查时间冲突
	bool isTimeConflict(const TimeSlot& newTimeSlot) const {
		for (Course* course : allCourses) {
			const CourseClass* selectedClass = course->getSelectedClass();
			if (selectedClass && selectedClass->timeSlot.isConflict(newTimeSlot)) {
				return true;
			}
		}
		return false;
	}
	
	// 设置控制台颜色
	void setColor(int color) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
	}
	
public:
	// 初始化总学分为 0，和课程数据
	CourseSystem() : totalCredits(0) {
		initializeCourses();
	}
	
	~CourseSystem() {
		for (Course* course : allCourses) {
			delete course;
		}
	}
	
	// 初始化课程数据
	void initializeCourses() {
		// 第一学期课程
		Course* c1 = new Course("CS101", "计算机导论", 2, 1);
		Course* c2 = new Course("CS102", "C语言程序设计", 3, 1);
		Course* c3 = new Course("MA101", "高等数学A(1)", 5, 1);
		Course* c4 = new Course("EN101", "大学英语(1)", 3, 1);
		Course* c5 = new Course("PH101", "大学物理(1)", 4, 1);
		Course* c6 = new Course("PE101", "体育(1)", 1, 1);
		Course* c7 = new Course("PS101", "思想道德修养", 2, 1);
		Course* c8 = new Course("CS103", "离散数学", 4, 1);
		
		// 第二学期课程
		Course* c9 = new Course("CS201", "数据结构", 4, 2);
		Course* c10 = new Course("CS202", "面向对象程序设计", 3, 2);
		Course* c11 = new Course("MA102", "高等数学A(2)", 5, 2);
		Course* c12 = new Course("EN102", "大学英语(2)", 3, 2);
		Course* c13 = new Course("PH102", "大学物理(2)", 4, 2);
		Course* c14 = new Course("PE102", "体育(2)", 1, 2);
		Course* c15 = new Course("CS203", "数字逻辑", 3, 2);
		Course* c16 = new Course("CS204", "算法设计与分析", 4, 2);
		
		// 体育(1)
		c6->addCourseClass(CourseClass("PE101-B", "张教练", TimeSlot("周五", "14:00", "15:00", "羽毛球场")));
		c6->addCourseClass(CourseClass("PE101-P", "李教练", TimeSlot("周三", "10:00", "11:00", "乒乓球馆")));
		c6->addCourseClass(CourseClass("PE101-BB", "王教练", TimeSlot("周二", "16:00", "17:00", "篮球场")));
		c6->addCourseClass(CourseClass("PE101-F", "赵教练", TimeSlot("周四", "08:00", "09:00", "足球场")));
		
		// 大学英语(1)
		c4->addCourseClass(CourseClass("EN101-R1", "刘老师", TimeSlot("周一", "09:00", "10:00", "外语楼201")));
		c4->addCourseClass(CourseClass("EN101-R2", "陈老师", TimeSlot("周四", "14:00", "15:00", "外语楼302")));
		
		// 大学物理(1)
		c5->addCourseClass(CourseClass("PH101-1", "黄教授", TimeSlot("周五", "14:00", "15:00", "物理实验楼101")));
		c5->addCourseClass(CourseClass("PH101-2", "吴教授", TimeSlot("周二", "10:00", "11:00", "物理实验楼202")));
		
		// 其他课程
		c1->addCourseClass(CourseClass("CS101-1", "马老师", TimeSlot("周一", "10:00", "11:00", "一号教学楼101")));
		c2->addCourseClass(CourseClass("CS102-1", "周老师", TimeSlot("周三", "14:00", "15:00", "二号教学楼202")));
		c3->addCourseClass(CourseClass("MA101-1", "郑老师", TimeSlot("周二", "08:00", "09:00", "三号教学楼303")));
		c7->addCourseClass(CourseClass("PS101-1", "孙老师", TimeSlot("周四", "10:00", "11:00", "四号教学楼404")));
		c8->addCourseClass(CourseClass("CS103-1", "朱老师", TimeSlot("周一", "10:00", "11:00", "二号教学楼105")));
		
		// 第二学期课程教学班
		c9->addCourseClass(CourseClass("CS201-1", "林老师", TimeSlot("周一", "14:00", "15:00", "二号教学楼201")));
		c10->addCourseClass(CourseClass("CS202-1", "高老师", TimeSlot("周三", "08:00", "09:00", "二号教学楼302")));
		c11->addCourseClass(CourseClass("MA102-1", "梁老师", TimeSlot("周四", "10:00", "11:00", "三号教学楼103")));
		c12->addCourseClass(CourseClass("EN102-1", "钟老师", TimeSlot("周二", "14:00", "15:00", "外语楼401")));
		c13->addCourseClass(CourseClass("PH102-1", "徐老师", TimeSlot("周五", "08:00", "09:00", "物理实验楼301")));
		c14->addCourseClass(CourseClass("PE102-1", "韩教练", TimeSlot("周一", "16:00", "17:00", "游泳馆")));
		c15->addCourseClass(CourseClass("CS203-1", "胡老师", TimeSlot("周二", "16:00", "17:00", "一号教学楼205")));
		c16->addCourseClass(CourseClass("CS204-1", "沈老师", TimeSlot("周四", "16:00", "17:00", "二号教学楼305")));
		
		// 设置依赖关系（！！核心代码！！）
		c9->addPrerequisite(c2);  c9->addPrerequisite(c8);
		c10->addPrerequisite(c2);
		c11->addPrerequisite(c3);
		c12->addPrerequisite(c4);
		c13->addPrerequisite(c5);
		c14->addPrerequisite(c6);
		c15->addPrerequisite(c8);
		c16->addPrerequisite(c9); c16->addPrerequisite(c10);
		
		allCourses = {c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16};
	}
	
	// 显示系统头部
	void displayHeader() {
		system("cls");				// 清屏
		system("chcp 936 > nul");	// 设置中文编码
		setColor(11);
		cout << "==============================================学生选课系统==============================================" << endl;
		setColor(14);
		cout << "颜色说明：";
		setColor(10); cout << "【绿色=已选】";
		setColor(14); cout << " | ";
		setColor(14); cout << "【黄色=可选】";
		setColor(14); cout << " | ";
		setColor(12); cout << "【红色=需先修课程】";
		setColor(7);
		cout << "\n========================================================================================================" << endl;
	}
	
	// 显示所有课程
	void displayCourses() {
		// 第一学期课程
		setColor(15);
		cout << "\n【第一学期课程】" << endl;
		cout << "----------------------------------------------------------------------------------------" << endl;
		for (int i = 0; i < 8; i++) {
			Course* c = allCourses[i];
			
			// 绿色（已选）、黄色（可选）、红色（有前置）
			if (c->hasSelectedClass()) setColor(10);
			else if (c->canSelect(allCourses)) setColor(14);
			else setColor(12);
			
			cout << "[" << (i + 1 < 10 ? "0" : "") << i+1 << "] " << setw(18) << left << c->name 
			<< " 学分:" << c->credit << "  ID:" << c->id << endl;
			
			// 显示该课程的所有教学班
			setColor(7);
			for (size_t j = 0; j < c->classes.size(); j++) {
				const auto& cls = c->classes[j];
				cout << "  └─ [" << j+1 << "] " << cls.classId << " | " << cls.teacher << " | "
					 << cls.timeSlot.weekday << " " << cls.timeSlot.startTime << "-" 
					 << cls.timeSlot.endTime << " | " << cls.timeSlot.location;
				if (cls.selected) {
					setColor(10); 
					cout << " 【已选】"; 
					setColor(7);
				}
				cout << endl;
			}
		}
		
		// 第二学期课程
		setColor(15);
		cout << "\n【第二学期课程】" << endl;
		cout << "----------------------------------------------------------------------------------------" << endl;
		for (int i = 8; i < 16; i++) {
			Course* c = allCourses[i];
			if (c->hasSelectedClass()) setColor(10);
			else if (c->canSelect(allCourses)) setColor(14);
			else setColor(12);
			
			cout << "[" << (i + 1 < 10 ? "0" : "") << i+1 << "] " << setw(18) << left << c->name 
			<< " 学分:" << c->credit << "  ID:" << c->id << endl;
			
			// 显示该课程的所有教学班
			setColor(7);
			for (size_t j = 0; j < c->classes.size(); j++) {
				const auto& cls = c->classes[j];
				cout << "  └─ [" << j+1 << "] " << cls.classId << " | " << cls.teacher << " | "
					 << cls.timeSlot.weekday << " " << cls.timeSlot.startTime << "-" 
				     << cls.timeSlot.endTime << " | " << cls.timeSlot.location;
				if (cls.selected) {
					setColor(10); 
					cout << " 【已选】"; 
					setColor(7);
				}
				cout << endl;
			}
		}
		setColor(7);
	}
	
	// 显示已选课程
	void displaySelectedCourses() {
		setColor(11);
		cout << "\n\n==============================================已选课程==============================================" << endl;
		bool hasSelected = false;
		totalCredits = 0;
		
		for (Course* course : allCourses) {
			if (course->hasSelectedClass()) {
				hasSelected = true;
				const CourseClass* cls = course->getSelectedClass();
				setColor(10);
				cout << "[" << course->id << "] " << setw(18) << left << course->name 
				<< " 学分:" << course->credit << "  学期:" << course->semester << endl;
				setColor(7);
				cout << "  教学班：" << cls->classId << " | 教师：" << cls->teacher << " | 时间：" 
				<< cls->timeSlot.weekday << " " << cls->timeSlot.startTime << "-" << cls->timeSlot.endTime
				<< " | 地点：" << cls->timeSlot.location << endl;
				totalCredits += course->credit;
			}
		}
		
		if (!hasSelected) {
			cout << "暂无已选课程" << endl;
		}
		
		setColor(14);
		cout << "\n总学分：" << totalCredits << " / 要求：" << CREDIT_THRESHOLD;
		if (totalCredits >= CREDIT_THRESHOLD) {
			setColor(10); cout << " ^V^ 已达标";
		} else {
			setColor(12); cout << " (╥﹏╥) 还需" << (CREDIT_THRESHOLD - totalCredits) << "学分";
		}
		setColor(7);
		cout << "\n========================================================================================================" << endl;
	}
	
	// 显示操作菜单
	void displayMenu() {
		setColor(15);
		cout << "\n操作菜单：" << endl;
		cout << "1-16：选择课程 | R：退选课程 | C：学分检查 | D：课程依赖 | T：查询课表 | Q：退出" << endl;
		cout << "请输入操作：";
	}
	
	// 选择课程
	void selectCourse(int courseIndex) {
		if (courseIndex < 1 || courseIndex > 16) {
			cout << "无效的课程编号！" << endl;
			Sleep(1500);	// 停留1.5秒
			return;
		}
		
		Course* course = allCourses[courseIndex - 1];
		
		// 检查前置课程（核心代码！！！）
		if (!course->canSelect(allCourses)) {
			setColor(12);
			cout << "QAQ 无法选择【" << course->name << "】，需先修：";
			for (Course* prereq : course->prerequisites) {
				if (!prereq->hasSelectedClass()) {
					cout << prereq->name << " ";
				}
			}
			setColor(7);
			cout << endl;
			Sleep(2000);
			return;
		}
		
		// 显示该课程的教学班列表
		system("cls");
		setColor(11);
		cout << "==============================================选择教学班==============================================" << endl;
		setColor(14);
		cout << "课程：" << course->name << "（学分：" << course->credit << "）" << endl;
		cout << "----------------------------------------------------------------------------------------" << endl;
		for (size_t i = 0; i < course->classes.size(); i++) {
			const auto& cls = course->classes[i];
			cout << "[" << i+1 << "] " << cls.classId << " | 教师：" << cls.teacher << " | 时间："
			<< cls.timeSlot.weekday << " " << cls.timeSlot.startTime << "-" << cls.timeSlot.endTime
			<< " | 地点：" << cls.timeSlot.location << endl;
		}
		cout << "输入教学班编号（0取消）：";
		
		int classChoice;
		cin >> classChoice;
		if (classChoice == 0) return;
		if (classChoice < 1 || classChoice > (int)course->classes.size()) {
			cout << "无效的教学班编号！" << endl;
			Sleep(1500);
			return;
		}
		
		// 获取选中的教学班
		CourseClass& selectedClass = course->classes[classChoice - 1];
		
		// 检查是否已选该教学班
		if (selectedClass.selected) {
			cout << "已选择该教学班！" << endl;
			Sleep(1500);
			return;
		}
		
		// 取消该课程其他已选的教学班
		for (auto& cls : course->classes) {
			if (cls.selected) {
				cls.selected = false;
				totalCredits -= course->credit;
			}
		}
		
		// 检查时间冲突
		if (isTimeConflict(selectedClass.timeSlot)) {
			setColor(12);
			cout << "时间冲突！该教学班与已选课程上课时间重叠，无法选择！" << endl;
			setColor(7);
			Sleep(2000);
			return;
		}
		
		// 选中该教学班
		selectedClass.selected = true;
		totalCredits += course->credit;
		
		setColor(10);
		cout << "^-^ 成功选择：" << course->name << " - " << selectedClass.classId << endl;
		setColor(7);
		Sleep(2000);
	}
	
	// 退选课程
	void removeCourse() {
		// 收集已选课程
		vector<Course*> selectedCourses;
		for (Course* course : allCourses) {
			if (course->hasSelectedClass()) {
				selectedCourses.push_back(course);
			}
		}
		
		if (selectedCourses.empty()) {
			cout << "无课程可退！" << endl;
			Sleep(1500);
			return;
		}
		
		// 显示已选课程列表
		system("cls");
		setColor(11);
		cout << "==============================================退选课程==============================================" << endl;
		for (size_t i = 0; i < selectedCourses.size(); i++) {
			Course* c = selectedCourses[i];
			const CourseClass* cls = c->getSelectedClass();
			cout << "[" << i+1 << "] " << c->name << " | " << cls->classId << " | "
			<< cls->timeSlot.weekday << " " << cls->timeSlot.startTime << "-" << cls->timeSlot.endTime << endl;
		}
		cout << "输入课程编号（0取消）：";
		
		int choice;
		cin >> choice;
		if (choice == 0) return;
		if (choice < 1 || choice > (int)selectedCourses.size()) {
			cout << "无效选择！" << endl;
			Sleep(1500);
			return;
		}
		
		// 获取目标课程
		Course* target = selectedCourses[choice - 1];
		vector<Course*> dependents;
		findDependentCourses(target, dependents);
		
		// 联动退选的提示
		if (!dependents.empty()) {
			setColor(12);
			cout << "\n警告：退选【" << target->name << "】会导致以下课程失去前置条件，需一同退选：" << endl;
			for (size_t i = 0; i < dependents.size(); i++) {
				cout << "  [" << i+1 << "] " << dependents[i]->name << endl;
			}
			setColor(7);
			cout << "是否确认退选（Y/N）：";
			char confirm;
			cin >> confirm;
			if (toupper(confirm) != 'Y') {		// 不区分大小写
				cout << "取消退选！" << endl;
				Sleep(1500);
				return;
			}
		}
		
		// 退选依赖课程
		for (Course* dep : dependents) {
			for (auto& cls : dep->classes) {
				if (cls.selected) {
					cls.selected = false;
					totalCredits -= dep->credit;
					cout << "@A@ 已退选依赖课程：" << dep->name << " - " << cls.classId << endl;
				}
			}
		}
		
		// 退选目标课程
		for (auto& cls : target->classes) {
			if (cls.selected) {
				cls.selected = false;
				totalCredits -= target->credit;
				cout << "@V@ 已成功退选：" << target->name << " - " << cls.classId << endl;
			}
		}
		
		Sleep(2000);
	}
	
	// 学分检查
	void checkRequirements() {
		system("cls");
		setColor(11);
		cout << "==============================================学分检查==============================================" << endl;
		if (totalCredits >= CREDIT_THRESHOLD) {
			setColor(10);
			cout << "恭喜^-^ 已选" << totalCredits << "学分，满足要求！" << endl;
		} else {
			setColor(12);
			cout << "警告！已选" << totalCredits << "学分，还需" << (CREDIT_THRESHOLD - totalCredits) << "学分！" << endl;
		}
		setColor(7);
		cout << "\n按任意键返回...";
		_getch();
	}
	
	// 显示课程依赖
	void displayDependencyGraph() {
		system("cls");
		setColor(11);
		cout << "==============================================课程依赖==============================================" << endl;
		for (Course* c : allCourses) {
			setColor(14);
			cout << c->name << " → 前置：";
			setColor(7);
			for (Course* p : c->prerequisites) cout << p->name << " ";
			cout << endl;
		}
		setColor(7);
		cout << "\n按任意键返回...";
		_getch();
	}
	
	// 查询课表
	void displayTimetable() {
		system("cls");
		setColor(11);
		cout << "==============================================我的课表==============================================" << endl;
		
		// 课表基础配置：星期和时间段
		vector<string> weekdays = {"周一", "周二", "周三", "周四", "周五"};
		vector<string> timeRanges = {"08:00-09:00", "09:00-10:00", "10:00-11:00", "14:00-15:00", "16:00-17:00"};
		
		// 第一学期
		setColor(14);
		cout << "\n【第一学期课表】" << endl;
		setColor(15);
		// 初始化第一学期课表数据
		vector<vector<pair<Course*, CourseClass*>>> timetableSem1(
			timeRanges.size(), 
			vector<pair<Course*, CourseClass*>>(weekdays.size(), {nullptr, nullptr})
			);
		unordered_map<string, int> courseIdToNum1; // 第一学期课程编号映射（课程代码-编号）
		int num1 = 1;
		
		// 绘制第一学期课表
		for (Course* course : allCourses) {
			if (course->semester != 1) continue;  // 仅处理第一学期课程
			const CourseClass* cls = course->getSelectedClass();
			if (cls) {
				// 查找时间段
				int timeIdx = -1;
				string timeRange = cls->timeSlot.startTime + "-" + cls->timeSlot.endTime;
				for (size_t i = 0; i < timeRanges.size(); i++) {
					if (timeRanges[i] == timeRange) {
						timeIdx = i;
						break;
					}
				}
				// 查找星期
				int dayIdx = -1;
				for (size_t i = 0; i < weekdays.size(); i++) {
					if (weekdays[i] == cls->timeSlot.weekday) {
						dayIdx = i;
						break;
					}
				}
				if (timeIdx != -1 && dayIdx != -1) {
					timetableSem1[timeIdx][dayIdx] = {course, const_cast<CourseClass*>(cls)};
					string key = course->id + "-" + cls->classId;
					if (courseIdToNum1.find(key) == courseIdToNum1.end()) {
						courseIdToNum1[key] = num1++;
					}
				}
			}
		}
		
		// 绘制第一学期课表表头
		cout << setw(12) << left << "时间段";
		for (const string& day : weekdays) {
			cout << setw(20) << left << day;
		}
		cout << endl;
		cout << string(12 + 20 * weekdays.size(), '-') << endl;
		
		// 绘制第一学期课表内容
		for (int i = 0; i < (int)timeRanges.size(); i++) {
			setColor(15);
			cout << setw(12) << left << timeRanges[i];
			for (int j = 0; j < (int)weekdays.size(); j++) {
				auto& entry = timetableSem1[i][j];
				if (entry.first && entry.second) {
					string key = entry.first->id + "-" + entry.second->classId;
					setColor(10);
					cout << setw(20) << left << ("[" + to_string(courseIdToNum1[key]) + "]" + entry.first->name);
					setColor(7);
				} else {
					cout << setw(20) << left << "";
				}
			}
			cout << endl;
		}
		
		// 第一学期课程编号说明
		if (!courseIdToNum1.empty()) {
			cout << "\n【第一学期课程编号说明】" << endl;
			for (const auto& pair : courseIdToNum1) {
				cout << "[" << pair.second << "] " << pair.first << endl;
			}
		} else {
			cout << "\n第一学期暂无已选课程" << endl;
		}
		
		// 第二学期课表绘制
		setColor(14);
		cout << "\n\n【第二学期课表】" << endl;
		setColor(15);
		// 初始化第二学期课表数据
		vector<vector<pair<Course*, CourseClass*>>> timetableSem2(
			timeRanges.size(), 
			vector<pair<Course*, CourseClass*>>(weekdays.size(), {nullptr, nullptr})
			);
		unordered_map<string, int> courseIdToNum2; // 第二学期课程编号映射
		int num2 = 1;
		
		// 填充第二学期课表
		for (Course* course : allCourses) {
			if (course->semester != 2) continue; // 仅处理第二学期课程
			const CourseClass* cls = course->getSelectedClass();
			if (cls) {
				// 查找时间段索引
				int timeIdx = -1;
				string timeRange = cls->timeSlot.startTime + "-" + cls->timeSlot.endTime;
				for (size_t i = 0; i < timeRanges.size(); i++) {
					if (timeRanges[i] == timeRange) {
						timeIdx = i;
						break;
					}
				}
				// 查找星期索引
				int dayIdx = -1;
				for (size_t i = 0; i < weekdays.size(); i++) {
					if (weekdays[i] == cls->timeSlot.weekday) {
						dayIdx = i;
						break;
					}
				}
				if (timeIdx != -1 && dayIdx != -1) {
					timetableSem2[timeIdx][dayIdx] = {course, const_cast<CourseClass*>(cls)};
					string key = course->id + "-" + cls->classId;
					if (courseIdToNum2.find(key) == courseIdToNum2.end()) {
						courseIdToNum2[key] = num2++;
					}
				}
			}
		}
		
		// 绘制第二学期课表表头
		cout << setw(12) << left << "时间段";
		for (const string& day : weekdays) {
			cout << setw(20) << left << day;
		}
		cout << endl;
		cout << string(12 + 20 * weekdays.size(), '-') << endl;
		
		// 绘制第二学期课表内容
		for (int i = 0; i < (int)timeRanges.size(); i++) {
			setColor(15);
			cout << setw(12) << left << timeRanges[i];
			for (int j = 0; j < (int)weekdays.size(); j++) {
				auto& entry = timetableSem2[i][j];
				if (entry.first && entry.second) {
					string key = entry.first->id + "-" + entry.second->classId;
					setColor(10);
					cout << setw(20) << left << ("[" + to_string(courseIdToNum2[key]) + "]" + entry.first->name);
					setColor(7);
				} else {
					cout << setw(20) << left << "";
				}
			}
			cout << endl;
		}
		
		// 第二学期课程编号说明
		if (!courseIdToNum2.empty()) {
			cout << "\n【第二学期课程编号说明】" << endl;
			for (const auto& pair : courseIdToNum2) {
				cout << "[" << pair.second << "] " << pair.first << endl;
			}
		} else {
			cout << "\n第二学期暂无已选课程" << endl;
		}
		
		// 课程详情查询
		cout << "\n==============================================课程详情查询==============================================" << endl;
		cout << "输入课程编号（格式：学期编号-课程编号，如1-1/2-3，0返回）：";
		string queryInput;
		cin >> queryInput;
		if (queryInput == "0") return;
		
		// 解析查询输入
		size_t dashPos = queryInput.find('-');
		if (dashPos == string::npos) {	// 查找是否正确输入了'-'
			cout << "输入格式错误！正确格式：学期编号-课程编号（如1-1）" << endl;
			Sleep(2000);
			return;
		}
		int semNum = stoi(queryInput.substr(0, dashPos));		// 学期
		int courseNum = stoi(queryInput.substr(dashPos + 1));	// 编号
		
		// 查找对应课程
		string targetKey;
		if (semNum == 1) {
			for (const auto& pair : courseIdToNum1) {
				if (pair.second == courseNum) {
					targetKey = pair.first;
					break;
				}
			}
		} else if (semNum == 2) {
			for (const auto& pair : courseIdToNum2) {
				if (pair.second == courseNum) {
					targetKey = pair.first;
					break;
				}
			}
		} else {
			cout << "无效的学期编号！仅支持1或2" << endl;
			Sleep(2000);
			return;
		}
		
		if (targetKey.empty()) {
			cout << "无效的课程编号！" << endl;
			Sleep(1500);
			return;
		}
		
		// 显示课程详情
		cout << "\n==============================================课程详情==============================================" << endl;
		for (Course* course : allCourses) {
			for (const auto& cls : course->classes) {
				string key = course->id + "-" + cls.classId;
				if (key == targetKey) {
					setColor(14);
					cout << "课程名称：" << course->name << endl;
					cout << "课程编号：" << course->id << endl;
					cout << "教学班：" << cls.classId << endl;
					cout << "授课教师：" << cls.teacher << endl;
					cout << "上课时间：" << cls.timeSlot.weekday << " " << cls.timeSlot.startTime << "-" << cls.timeSlot.endTime << endl;
					cout << "教学地点：" << cls.timeSlot.location << endl;
					cout << "学分：" << course->credit << endl;
					cout << "所属学期：第" << course->semester << "学期" << endl;
					setColor(7);
					break;
				}
			}
		}
		
		cout << "\n按任意键返回...";
		_getch();
	}
};

// 主函数
int main() {
	system("mode con cols=150 lines=40");
	system("title 学生选课系统");
	system("chcp 936 > nul");
	
	CourseSystem courseSystem;
	string inputStr;
	bool isQuit = false;		// 退出标记
	
	while (!isQuit) {
		// 绘制 UI
		courseSystem.displayHeader();
		courseSystem.displayCourses();
		courseSystem.displaySelectedCourses();
		courseSystem.displayMenu();
		
		// 清空输入流
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		getline(cin, inputStr);
		
		// 空输入为无效操作
		if (inputStr.empty()) {
			cout << "无效操作！" << endl;
			Sleep(1500);
			continue;
		}
		
		// 数字输入（1-16）
		bool isNumber = true;
		int index = 0;
		try {
			index = stoi(inputStr);
		} catch (...) {
			isNumber = false;			// 非数字输入
		}
		
		if (isNumber) {
			if (index >= 1 && index <= 16) {
				courseSystem.selectCourse(index);
			} else {
				cout << "无效操作！" << endl;
				Sleep(1500);
			}
		}
		// 字母输入
		else if (inputStr.length() == 1 && isalpha(inputStr[0])) {
			char choice = toupper(inputStr[0]);
			switch (choice) {
				case 'R': courseSystem.removeCourse(); break;							// 退选
				case 'C': courseSystem.checkRequirements(); break;						// 查学分
				case 'D': courseSystem.displayDependencyGraph(); break;					// 查依赖
				case 'T': courseSystem.displayTimetable(); break;						// 查课表
				case 'Q': cout << "感谢使用！"; Sleep(1000); isQuit = true; break;		// 退出
				default: cout << "无效操作！"; Sleep(1500); break;
			}
		}
		// 其他输入
		else {
			cout << "无效操作！" << endl;
			Sleep(1500);
		}
	}
	
	return 0;
}