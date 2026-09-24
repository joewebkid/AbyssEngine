# Следующая цель Gemini Pro: закрытие семейства Mesh loader

## Почему это продолжение, а не новое семейство

Коммит `8dbbfaf3` улучшил `MeshReadData` и `ReadEnhancedDataFromFile`, но исходная цель ещё не закрыта: нужен законченный `MeshCreateFromFile`, единый focused-отчёт трёх функций, полный ARM corpus и отдельный доказательный документ. До этого переход к `LevelScript` запрещён.

## Главная цель

Довести семейство `MeshCreateFromFile` / `MeshReadData` / `Mesh::ReadEnhancedDataFromFile` до доказанного финального состояния: стремиться к `99%` ARM source-shape, но принимать подтверждённые компиляторные потолки; проверить каждый крупный пакет focused verify и полный ARM corpus, сохранить зелёную native-сборку и документировать все принятые и отклонённые формы.

## Задания

1. Удалить trailing whitespace в принятом коде и убедиться, что scratch-файлы, `ndk.zip`, временные скрипты и дизассемблерные выгрузки не попадут в Git.
2. Снять свежий baseline после `8dbbfaf3` для всех трёх функций. Показать source-shape, strict fuzzy, instruction counts, instruction-weighted среднее и exact status.
3. Завершить `MeshCreateFromFile`: signature/header lifetime, handle ownership, single/multimesh join, child construction, material/output ownership, failure cleanup и register lifetime.
4. Повторно проверить принятые формы `MeshReadData` и `ReadEnhancedDataFromFile` именно focused verifier, а не только native build. Не называть `91.4/91.7%` абсолютным потолком без сохранённого сравнения и конкретного compiler-difference evidence.
5. Запустить полный ARM corpus. Сравнить exact-счётчики и список регрессий с baseline ветки; ни одна потеря exact не допускается без адресного доказательства исправления семантики.
6. Проверить UCRT64 native build и ARM current-source coverage.
7. Создать `docs/MESH_LOADER_SOURCE_SHAPE_ARM_2026-09-24.md`: адреса, baseline/final, per-function и weighted метрики, accepted/rejected trials, compiler ceilings, полный corpus, native/ARM build.
8. Закоммитить только исходники и итоговую документацию. `_work`, `ndk.zip`, scratch/test-файлы и оригинальные бинарники оставить неотслеживаемыми.

## Ограничения

- Не возвращать ранее отклонённые broad error tails, искусственные stack locals, volatile/register tricks, compiler barriers или мёртвый control flow ради метрики.
- `-fno-vectorize`, отдельные compiler flags или pragma допустимы только если это доказанная настройка исходного Android translation unit; иначе не принимать.
- Не менять `StarMap`, TextureCreate, `LevelScript` и Godot.
- Если корректная форма остаётся ниже `99%`, зафиксировать потолок и перейти дальше только после выполнения всех проверок выше.

## Следующая очередь после закрытия

После принятого финального отчёта следующим отдельным заданием будет семейство `LevelScript`, начиная с `LevelScript::process(int)` и конструкторов. Не смешивать его с текущим Mesh-коммитом.
