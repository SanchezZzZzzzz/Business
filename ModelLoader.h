#include<vector>
#include"glew.h"
#include"glut.h"
#include<fstream>
#include<string>
#include<iostream>
#include<cmath>
using namespace std;
namespace ml {
    struct Material { //Структура, которая представляет материал
    public:
        string name; //Имя материала
        GLfloat Reflection_coefficient; //Коэффициент отражения
        vector<GLfloat>AmbientLightColor; //Цвет окружающего освещения
        vector<float>DiffuseColor; //Цвет материала так сказатб
        vector<GLfloat>ReflectionColor; //Цвет отражения
        float Transparency; //Прозрачность

    };
    struct face { //Структура, представляющая грань
    public:
        vector<int>vi = {}; //Индексы вершин
        vector<int>ti = {}; //индексы координатных текстур
        vector<int>ni = {}; //Индексы нормали
        vector<float>NormalVector = {}; //Вектор нормали
        Material* FaceMaterial; //Материал грани
    };
    struct Object { //Структура, которая содержит данные о конкретном объекте
    public:
        string name; //Имя объекта
        vector<vector<float>>vertex; //Все вершины
        vector<vector<float>>tcoords; //Все текстурные координаты
        vector<vector<float>>normals; // Все нормали
        vector<face>faces; //Все грани объекта
    };
    class Model {
    public:
        vector<Object>objects; //Все объекты модели
        vector<Material>matProps; //Все материалы модели
        string materials = ""; //Файл с материалами
        GLfloat ModelCoords[3] = { 0, 0, 0 };
        GLfloat ModelRot[3] = {0, 0, 0};
        void Import_objects(string Fname) { //Метод импорта модели, Fname - obj файл для импортп
            int preV = 0, preT = 0, preN = 0; //индексы для смещения при новом объекте
            int postV = 0, postT = 0, postN = 0; //Принимают Pre-значения для нового объекта
            const string tempFname = Fname; //Имя файла в const строке
            ifstream obj(tempFname); //Файл объектов в obj
            if (obj.is_open()) { //Если открыт
                string parsed; //здесь будет храниться каждая строка
                Object temp; //Новый временный объект
                Material* LastMaterial = &*new Material; //Последний материал будет присваиваться к новой грани
                bool ObjectFound = 0;//Равен 0 до первого объекта
                while (getline(obj, parsed)) { //Парсинг obj
                    if (parsed[0] == 'o') {//Новый объект найден
                        if (ObjectFound) //Если это не первый объект
                            objects.push_back(temp); //Добавляем его ко всем объектам
                        temp = *new Object; //temp теперь новый
                        ObjectFound = 1; //Теперь первых объектов не будэ
                        temp.name = parsed.substr(parsed.find(" " + 1), parsed.size()); //Имя файла - всё кроме "o "
                        cout << "initializing object: " << temp.name << "\n"; //Типа круто когда всё выводится в консоль (ЛУЧШЕ НЕ СМОТРЕТЬ ЕСЛИ В ИМЕНИ ЕСТЬ КИРИЛЛИЦА)
                        postV = preV; //Ну тут собсна смещения смещаются
                        postT = preT; //И тут
                        postN = preN; //А ещё тут
                    }
                    else if (parsed.substr(0, parsed.find(" ")) == "mtllib") { //Если в строке файл материала
                        materials = parsed.substr(parsed.find(" ") + 1, parsed.size()); //Имя файла материалов тут
                        cout << "Yay, :D material file found. Name is: " << materials << "\n"; //Говорим что всё найдено (Это ещё не значит что всё будет работать)
                        Add_material(); //Добавляем материалы к модели

                    }
                    else if (parsed[0] != '#' && parsed[0] != 'm') { //если не ненужная строка
                        vector<float>coords = {}; //Координаты, которые могут быть чем угодно от вершин до нормалей
                        string tempstr = parsed; //Временная строка чтобы не портить parsed (хз зачем)
                        if (parsed.substr(0, parsed.find(" ")) == "v") { //Если строка это вершины
                            tempstr.erase(0, tempstr.find(" ") + 1); //Получаем координату по x
                            coords.push_back(stof(tempstr.substr(0, tempstr.find(" ")))); //Запихиваем коордирату в 0 элемент
                            tempstr.erase(0, tempstr.find(" ") + 1); //С остальными так же
                            coords.push_back(stof(tempstr.substr(0, tempstr.find(" "))));
                            tempstr.erase(0, tempstr.find(" ") + 1);
                            coords.push_back(stof(tempstr));
                            temp.vertex.push_back(coords); //вершина добавляется в temp
                            preV++; //Прибавляем 1, так как объекты продолжают индексироваться для следующих объектов в obj файле
                        }
                        else if (parsed.substr(0, parsed.find(" ")) == "vt") { //Тот же алгоритм с текстурными координатами
                            tempstr.erase(0, tempstr.find(" ") + 1);
                            coords.push_back(stof(tempstr.substr(0, tempstr.find(" "))));
                            tempstr.erase(0, tempstr.find(" ") + 1);
                            coords.push_back(stof(tempstr.substr(0, tempstr.find(" "))));
                            tempstr.erase(0, tempstr.find(" ") + 1);
                            coords.push_back(stof(tempstr));
                            temp.tcoords.push_back(coords);
                            preT++;
                        }
                        else if (parsed.substr(0, parsed.find(" ")) == "vn") { //И нормальными
                            tempstr.erase(0, tempstr.find(" ") + 1);
                            coords.push_back(stof(tempstr.substr(0, tempstr.find(" "))));
                            tempstr.erase(0, tempstr.find(" ") + 1);
                            coords.push_back(stof(tempstr.substr(0, tempstr.find(" "))));
                            tempstr.erase(0, tempstr.find(" ") + 1);
                            coords.push_back(stof(tempstr));
                            temp.normals.push_back(coords);
                            preN++;
                        }
                        else if (parsed.substr(0, parsed.find(" ")) == "f") { //Андрюха, у нас грань, возможно криминал
                            tempstr.erase(0, tempstr.find(" ") + 1); //стираем всё лишнее
                            face Tface = {}; //Временная грань
                            Tface.FaceMaterial = &*LastMaterial; //Материал равен последнему найденному
                            while (tempstr != "") { //Пока строка не пуста
                                try { //Пытаемся, так как не факт что нормально выйдет, хоть я уже пофиксил баг
                                    if (tempstr.find(" ") != string::npos) { //Если пробел таки есть
                                        string sface = tempstr.substr(0, tempstr.find(" ")); //Выбираем отдельную вершину
                                        if (sface.substr(0, sface.find("/")) != "") { //Если нашлась вершина
                                            Tface.vi.push_back(stoi(sface.substr(0, sface.find("/"))) - 1 - postV); //Добавляем к вершине новый индекс
                                            sface.erase(0, sface.find("/") + 1); //Стираем её
                                        }
                                        else
                                            sface.erase(0, sface.find("/") + 2); //Если не нашлось, то пропускаем
                                        if (sface.substr(0, sface.find("/")) != "") { //Аналогично вершинам
                                            Tface.ti.push_back(stoi(sface.substr(0, sface.find("/"))) - 1 - postT);
                                            sface.erase(0, sface.find("/") + 1);
                                        }
                                        else
                                            sface.erase(0, sface.find("/") + 1);
                                        sface.erase(0, sface.find("/") + 1); //Нормали просто всё остальное
                                        Tface.ni.push_back(stoi(sface) - 1 - postN);
                                    }
                                    else { //Тот же алгоритм с поправкой на то, что это последняя вершина
                                        if (tempstr.substr(0, tempstr.find("/")) != "") {
                                            Tface.vi.push_back(stoi(tempstr.substr(0, tempstr.find("/"))) - 1 - postV);
                                            tempstr.erase(0, tempstr.find("/") + 1);
                                        }
                                        else
                                            tempstr.erase(0, tempstr.find("") + 2);
                                        if (tempstr.substr(0, tempstr.find("/")) != "") {
                                            Tface.ti.push_back(stoi(tempstr.substr(0, tempstr.find("/"))) - 1 - postT);
                                            tempstr.erase(0, tempstr.find("/") + 1);
                                        }
                                        else
                                            tempstr.erase(0, tempstr.find("/") + 1);
                                        tempstr.erase(0, tempstr.find("/") + 1);
                                        Tface.ni.push_back(stoi(tempstr) - 1 - postN);
                                        tempstr = "";
                                    }
                                    tempstr.erase(0, tempstr.find(" ") + 1);
                                }
                                catch (...) {
                                    cout << "Error occured! (*__*;⁠)"; //А что если будет ошибка?
                                }
                            }
                            Tface.NormalVector = GetScalarVector(temp, Tface.ni); //Получаем нормальный вектор
                            temp.faces.push_back(Tface); //Теперь всё хорошо, добавляем грань ко временному объекту
                        }
                        else if (parsed.substr(0, parsed.find(" ")) == "usemtl") { //Если используется другой материал
                            LastMaterial = FindMaterialByName(matProps, parsed.substr(parsed.find(" ") + 1, parsed.size())); //Это у нас теперь последний
                        }
                    }
                }
                objects.push_back(temp); //Добавляем к объектам временный объект
            }
            else //А нет файла у нас!
                cout << "Error! Couldn't open the file!\n";
        }
        void Draw_objects(bool is_smooth, bool Enable_dark) { //Давайте рисовать!
            glPushMatrix();
            glTranslatef(ModelCoords[0], 0, 0);
            glTranslatef(0, 0, ModelCoords[2]);
            for (__int8 i = 0; i < objects.size(); i++) { //Перебор по объектам
                Object& objref = objects[i]; //Чтобы не усугублять ситуацию избавимся хотя бы от одного индекса
                for (int j = 0; j < objref.faces.size(); j++) { //Перебор по граням
                    glBegin(GL_POLYGON); //Начинаем рисовать полигон
                    if (!is_smooth) {
                        glShadeModel(GL_FLAT);
                        glNormal3f(objref.faces[j].NormalVector[0], objref.faces[j].NormalVector[1], objref.faces[j].NormalVector[2]); //Вектор нормали, вычисленный ранее
                    }
                    if (!Enable_dark) {
                        glColor4f(objref.faces[j].FaceMaterial->DiffuseColor[0], objref.faces[j].FaceMaterial->DiffuseColor[1], objref.faces[j].FaceMaterial->DiffuseColor[2], objref.faces[j].FaceMaterial->Transparency); //Материал
                        GLfloat Rc[] = { objref.faces[j].FaceMaterial->ReflectionColor[0], objref.faces[j].FaceMaterial->ReflectionColor[0], objref.faces[j].FaceMaterial->ReflectionColor[2] }; //Массив с цветом отражения
                        glMaterialfv(GL_FRONT, GL_SPECULAR, Rc);
                        glMateriali(GL_FRONT, GL_SHININESS, objref.faces[j].FaceMaterial->Reflection_coefficient);
                    }
                    else
                        glColor4f(0, 0, 0, 1);
                    for (unsigned __int8 k = 0; k < objref.faces[j].vi.size(); k++) { //Идём по вершинам
                        if (is_smooth) {
                            glShadeModel(GL_SMOOTH);
                            glNormal3f(*&objref.normals[objref.faces[j].ni[k]][0], *&objref.normals[objref.faces[j].ni[k]][1], *&objref.normals[objref.faces[j].ni[k]][2]);
                        }
                        glVertex3f(*&objref.vertex[objref.faces[j].vi[k]][0], *&objref.vertex[objref.faces[j].vi[k]][1], *&objref.vertex[objref.faces[j].vi[k]][2]); //Вершина тут
                    }
                    glEnd(); //Дорисовались
                }
            }
            glPopMatrix();
        }
        void Translate(GLfloat x, GLfloat y, GLfloat z) {
            ModelCoords[0] = x;
            ModelCoords[1] = y;
            ModelCoords[2] = z;
        }
    private:
        void Add_material() { //Добавления материала
            const string Mfile = materials; //Строка для имени материала
            ifstream MatFile(Mfile); //Читаем из файла
            Material temp; //Временный материал
            string parsed; //Строка для парсинга
            bool MaterialFound = false;
            if (MatFile.is_open()) {
                while (getline(MatFile, parsed)) {
                    try {
                        if (parsed.substr(0, parsed.find(" ")) == "newmtl") {
                            if (MaterialFound)
                                matProps.push_back(temp);
                            else
                                MaterialFound = 1;
                            temp = *new Material;
                            temp.name = parsed.substr(parsed.find(" ") + 1, parsed.size());
                        }
                        else if (parsed.substr(0, parsed.find(" ")) == "Ns")
                            temp.Reflection_coefficient = stof(parsed.substr(parsed.find(" ") + 1, parsed.size()));
                        else if (parsed.substr(0, parsed.find(" ")) == "Ka")
                            AddValsToArray(temp.AmbientLightColor, parsed);
                        else if (parsed.substr(0, parsed.find(" ")) == "Kd")
                            AddValsToArray(temp.DiffuseColor, parsed);
                        else if (parsed.substr(0, parsed.find(" ")) == "Ks")
                            AddValsToArray(temp.ReflectionColor, parsed);
                        else if (parsed.substr(0, parsed.find(" ")) == "d")
                            temp.Transparency = stof(parsed.substr(parsed.find(" ") + 1, parsed.size()));
                    }
                    catch (...) {
                        cout << "Error occured while reading: " << parsed << "\n";
                    }
                }
                matProps.push_back(temp);
            }
            else {
                cout << "Couldn't find Material file :(\n";
            }
        }
        void AddValsToArray(vector<float>& Property, string read_from) {
            read_from.erase(0, read_from.find(" ") + 1);
            Property.push_back(stof(read_from.substr(0, read_from.find(" "))));
            read_from.erase(0, read_from.find(" ") + 1);
            Property.push_back(stof(read_from.substr(0, read_from.find(" "))));
            read_from.erase(0, read_from.find(" ") + 1);
            Property.push_back(stof(read_from));
        }
        Material* FindMaterialByName(vector<Material>& Matlist, string Matname) {
            for (int i = 0; i < Matlist.size(); i++) {
                if (Matname == Matlist[i].name)
                    return &Matlist[i];
            }
            return &*new Material;
        }
        vector<float> GetScalarVector(Object& Normals, vector<int>& NormalCoords) {
            vector<float>ToReturn = { 0, 0, 0 };
            for (int i = 0; i < NormalCoords.size(); i++) {
                ToReturn[0] += Normals.normals[NormalCoords[i]][0];
                ToReturn[1] += Normals.normals[NormalCoords[i]][1];
                ToReturn[2] += Normals.normals[NormalCoords[i]][2];
            }
            return ToReturn;
        }
    };
}
