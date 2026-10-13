#include <cassert>
#include <cstdio>
#include <vector>

int buttonsDeleted = 0, partsDeleted = 0, stringsDeleted = 0;
int arraysReleased = 0, arraysDeleted = 0;
struct TouchButton { ~TouchButton() { ++buttonsDeleted; } };
struct ImagePart { ~ImagePart() { ++partsDeleted; } };
struct String { ~String() { ++stringsDeleted; } };
template <typename T> struct Array : std::vector<T> {
    ~Array() { ++arraysDeleted; }
};
template <typename T> void ArrayReleaseClasses(Array<T> &items) {
    ++arraysReleased;
    for (T item : items) delete item;
    items.clear();
}
struct StatusWindow {
    Array<TouchButton *> *tabButtons = nullptr;
    Array<TouchButton *> *medalButtons = nullptr;
    Array<ImagePart *> *imageParts = nullptr;
    Array<String *> *detailLines = nullptr;
    ~StatusWindow();
};

// PRODUCTION_DESTRUCTOR

int main() {
    {
        StatusWindow window;
        window.tabButtons = new Array<TouchButton *>();
        window.medalButtons = new Array<TouchButton *>();
        window.imageParts = new Array<ImagePart *>();
        window.detailLines = new Array<String *>();
        window.tabButtons->push_back(new TouchButton());
        window.tabButtons->push_back(new TouchButton());
        window.medalButtons->push_back(new TouchButton());
        window.imageParts->push_back(new ImagePart());
        window.detailLines->push_back(new String());
    }
    assert(buttonsDeleted == 3 && partsDeleted == 1 && stringsDeleted == 1);
    assert(arraysReleased == 4 && arraysDeleted == 4);
    { StatusWindow empty; }
    assert(arraysReleased == 4 && arraysDeleted == 4);
    std::puts("StatusWindow destructor host smoke: OK");
}
