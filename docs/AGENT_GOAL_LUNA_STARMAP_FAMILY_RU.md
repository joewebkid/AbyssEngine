# Следующая цель Luna: семейство StarMap

## Рабочая область

- Ветка: `codex/luna-texture-arm-closure`
- Worktree: `C:\Users\Dima\Documents\Galaxy on Fire new version\gof2hd-decomp-luna-arm`
- Начать после коммита `fc7a491e`.
- Предыдущий TextureCreate-пакет не открывать повторно без новой адресной ARM-улики.

## Главная цель

Поднять среднее ARM source-shape сопоставление семейства `StarMap` в `gof2hd-decomp` до не менее `99%`, используя подтверждённые Android/iOS/PC источники, проверяя каждый крупный пакет focused verify и полный ARM corpus, документируя принятые и отклонённые формы, сохраняя ARM coverage `204/204` и зелёную native-сборку.

Цель `99%` не разрешает улучшать только короткие функции. Итог обязан содержать невзвешенное и instruction-weighted среднее, strict fuzzy, instruction counts и результат каждой функции отдельно.

## Приоритетные функции

1. `StarMap::init(bool, Mission*, bool, int)`.
2. `StarMap::OnTouchEnd(int, int)` и `OnTouchBegin(int, int)`.
3. `StarMap::draw()` и `drawOnScreenInfo(int, bool)`.
4. `StarMap::update(int)` и `initStarSystem()`.
5. Конструкторы/деструкторы и короткие helpers — только после больших тел; exact-функции не ломать.

## Задания

1. Снять свежий baseline `^_ZN7StarMap`, полный corpus и native build. Зафиксировать Android-адреса и per-function метрики.
2. Построить карту полей и CFG по Android ARM: текущая система, выбранная система, mission target, touch/drag state, zoom/rotation, route/landmark arrays и экранные подписи.
3. Сверять iOS и PC только для имен/семантики; Android остаётся источником ABI, констант и ARM shape.
4. Работать пакетами: constructor/init, input, update/navigation, draw/info. После каждого пакета запускать focused verify.
5. После изменения заголовков, layout или общих типов запускать полный corpus и проверять `204/204`.
6. Принимать только доказанные формы. Для rejected trial сохранять метрики/причину в `_work`, но не оставлять пробную форму в исходнике.
7. Подготовить `docs/STARMAP_SOURCE_SHAPE_ARM_2026-09-24.md` с baseline/final, accepted/rejected и оставшимися потолками.

## Источники истины

1. `_work/bins/android_2.0.16_libgof2hdaa.so` и `_work/symbols/`.
2. `C:\Users\Dima\Documents\Galaxy on Fire new version\analysis\gof2_libgof2hdaa_full_ida.c`.
3. iOS IDA/Ghidra dumps в основном workspace.
4. DeepOpen допускается только для семантики карты и маршрутов, не для Android layout/ARM shape.

## Обязательные проверки

- ARM current-source coverage остаётся `204/204`.
- UCRT64 native build `gof2` проходит.
- Focused StarMap report и полный ARM corpus сохранены в `_work`.
- Не потеряны существующие linked-exact/byte-exact функции без доказанного исправления поведения.
- Запрещены synthetic spills, volatile/register barriers, фиктивные locals, мёртвые ветки и усреднение aliases ради процента.

## Граница ветки

Владение этой задачи: `src/game/world/StarMap.cpp`, `StarMap.h` и отдельный отчёт. Не менять `LevelScript`, `Level`, Mesh loader и TextureCreate. Необходимое общее изменение сначала описать как зависимость.

Если доказанный корректный вариант не достигает `99%`, сохранить его и документировать честный потолок. Семантика важнее метрики.
