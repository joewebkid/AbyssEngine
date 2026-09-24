# Цель для Gemini Pro / Antigravity: семейство Mesh loader

## Рабочая область

- Ветка: `codex/gemini-mesh-loader-recovery`
- Worktree: `C:\Users\Dima\Documents\Galaxy on Fire new version\gof2hd-decomp-gemini-mesh`
- Исходный снимок: все отслеживаемые изменения текущего decomp на момент создания ветки.
- Нельзя добавлять в Git оригинальные APK/SO, игровые ресурсы или содержимое `_work/bins`.

## Главная цель

Поднять среднее ARM source-shape сопоставление семейства нативных загрузчиков `MeshCreateFromFile` / `MeshReadData` / `Mesh::ReadEnhancedDataFromFile` в `gof2hd-decomp` до не менее `99%`, используя подтверждённые Android/iOS/PC источники, проверяя каждый крупный пакет focused verify и полный ARM corpus, документируя принятые и отклонённые формы и сохраняя зелёную native-сборку.

Среднее `99%` считается только вместе с per-function и instruction-weighted результатами. Нельзя поднять отчёт короткими aliases, оставив три большие функции низкими.

## Целевое семейство

- `AbyssEngine::MeshCreateFromFile`.
- `AbyssEngine::MeshReadData`.
- `AbyssEngine::Mesh::ReadEnhancedDataFromFile`.
- Непосредственные конструкторы, release/error helpers и `Transform`/animation calls включать только когда они меняют форму или семантику трёх главных тел.
- Сохранить уже принятые исправления `Mesh()`, `Mesh(Mesh*)`, `MeshReleaseIntern`, VBO wrappers и native animation range.

## Задания

1. Снять свежий baseline трёх функций и полного corpus; не использовать старые проценты из документа как текущий результат.
2. Построить карту Android CFG для file header, single/multimesh, version routing, raw/compressed streams, recursive children, material ownership и error cleanup.
3. Отдельно восстановить natural lifetime локальных `Vector`, `Transform`, buffer и hidden-return объектов без искусственных stack fillers.
4. Для `MeshReadData` закрыть общий raw-buffer/error routing, реальные границы локальных объектов и оставшиеся register/call-order расхождения.
5. Для `ReadEnhancedDataFromFile` закрыть failure path, keyframe/channel routing, временные Transform и естественный cleanup.
6. Для `MeshCreateFromFile` закрыть argument/register lifetime и точные join-точки single/multimesh веток.
7. Каждую крупную гипотезу проверять отдельным focused report. Отклонённую форму полностью убирать из исходника, сохраняя только diff/метрики и краткое объяснение в `_work`/итоговом документе.
8. После принятых ABI/header изменений запускать полный ARM corpus и native build.
9. Подготовить итоговый документ: Android-адреса, начальные/финальные метрики, принятые формы, rejected trials, остаточные несовпадения и честная граница byte-exact.

## Источники истины

1. Android ARM32 `libgof2hdaa.so` в `_work/bins` и таблицы `_work/symbols`.
2. `C:\Users\Dima\Documents\Galaxy on Fire new version\analysis\gof2_libgof2hdaa_full_ida.c`.
3. `C:\Users\Dima\Documents\Galaxy on Fire new version\analysis\gof2_gidra_ios_engine_full_source.c` и iOS IDA dump — перекрёстная проверка именованных методов и ownership.
4. `C:\Users\Dima\Documents\Galaxy on Fire new version\docs\findings\gof2_aem_mesh_loader_vbo_audit_20260825_ru.md` — журнал уже принятых и отклонённых форм.
5. Реальные AEM-файлы разрешены только для проверки формата; они не доказывают машинную форму функции.

## Обязательные проверки

- UCRT64 native build `gof2` проходит после каждого принятого пакета.
- Focused verify показывает каждую из трёх функций, невзвешенное и instruction-weighted среднее, strict fuzzy и instruction counts.
- Полный corpus не снижает существующие exact-счётчики без доказанного исправления поведения.
- Не добавлять synthetic spills, stack-canary scratch, compiler barriers, volatile/register tricks, фиктивные массивы или мёртвый control flow ради метрики.
- Не возвращать ранее отклонённые broad unified error tails, hand-ordered constructor stores, magic-failure `goto`, direct-Vector final-loop rewrites и другие формы, перечисленные в AEM-аудите, без нового адресного ARM-доказательства.

## Граница ветки

Не исправлять `Player`/`NewsTicker`/`StarMap` и не менять семейство TextureCreate: ими владеет ветка Luna. Зависимости записывать в отчёт, а не дублировать исправления.

Если корректная доказанная форма не достигает `99%`, сохранить семантически верный вариант и документировать потолок. Процент не является основанием ухудшать восстановленное поведение.
