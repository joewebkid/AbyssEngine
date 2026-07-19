

#include "engine/core/Array.h"
#include "engine/core/Node.h"
#include "game/ship/Agent.h"
#include "game/mission/PendingProduct.h"
#include "game/mission/BluePrint.h"
#include "engine/file/AELowLevelFile.h"
#include "game/ui/TouchButton.h"
#include "game/ui/TouchSlider.h"
#include "engine/render/ImagePart.h"
#include "game/ui/ListItem.h"
#include "game/ship/Player.h"

template void ArrayReleaseArrays<signed char *>(Array<signed char *> &);

template void ArrayRemove<Node *>(Node *, Array<Node *> &);

template void ArrayRemove<Agent *>(Agent *, Array<Agent *> &);

template void ArrayRelease<Node *>(Array<Node *> &);

template void ArrayRelease<bool>(Array<bool> &);

template void ArrayRelease<int>(Array<int> &);

template void ArraySetLength<PendingProduct *>(unsigned int, Array<PendingProduct *> &);

template void ArraySetLength<BluePrint *>(unsigned int, Array<BluePrint *> &);

template void ArraySetLength<signed char>(unsigned int, Array<signed char> &);

template void ArraySetLength<Array<ImagePart *> *>(unsigned int, Array<Array<ImagePart *> *> &);

template void ArrayReleaseClasses<AELowLevelFile *>(Array<AELowLevelFile *> &);

template void ArrayReleaseClasses<PendingProduct *>(Array<PendingProduct *> &);

template void ArrayReleaseClasses<Node *>(Array<Node *> &);

template void ArrayReleaseClasses<BluePrint *>(Array<BluePrint *> &);

template void ArrayReleaseClasses<Array<ListItem *> *>(Array<Array<ListItem *> *> &);

template void ArrayReleaseClasses<Array<ImagePart *> *>(Array<Array<ImagePart *> *> &);

template void ArrayAdd<TouchButton *>(TouchButton *, Array<TouchButton *> &);

template void ArrayAdd<TouchSlider *>(TouchSlider *, Array<TouchSlider *> &);

template void ArrayAdd<bool>(bool, Array<bool> &);

template void ArrayAdd<float>(float, Array<float> &);

template void ArrayAdd<unsigned char>(const unsigned char *, unsigned int, Array<unsigned char> &);

template void ArrayAdd<Player *>(Player * const *, unsigned int, Array<Player *> &);

template void ArrayAdd<Player *>(const Array<Player *> &, Array<Player *> &);

class GameRecord;

namespace AbyssEngine {
    class KeyCode;
    class String;
}

template Array<GameRecord *>::~Array();

template Array<TouchSlider *>::Array();

template Array<TouchSlider *>::~Array();

template Array<PendingProduct *>::~Array();

template Array<BluePrint *>::Array();

template Array<BluePrint *>::~Array();

template Array<AbyssEngine::KeyCode *>::Array();

template Array<AbyssEngine::KeyCode *>::~Array();

template Array<Array<ImagePart *> *>::Array();

template Array<Array<AbyssEngine::String *> *>::Array();

template Array<Array<AbyssEngine::String *> *>::~Array();

template Array<bool>::~Array();