# Цель для Luna: ARM 204/204 и семейство TextureCreate

## Рабочая область

- Ветка: `codex/luna-texture-arm-closure`
- Worktree: `C:\Users\Dima\Documents\Galaxy on Fire new version\gof2hd-decomp-luna-arm`
- Исходный снимок: все отслеживаемые изменения текущего decomp на момент создания ветки.
- Нельзя переносить в Git оригинальные APK/SO, игровые ресурсы или содержимое `_work/bins`.

## Главная цель

Сначала довести покрытие актуальной ARM-сборки с `201/204` до `204/204`, устранив подтверждённые конфликты `SolarSystem*`/`int` в `Player`, `NewsTicker` и `StarMap`. Затем поднять среднее ARM source-shape сопоставление семейства `AbyssEngine::TextureCreate*` / `PaintCanvas::TextureCreate*` до не менее `99%`, используя подтверждённые Android/iOS/PC источники, проверяя каждый крупный пакет focused verify и полный ARM corpus, документируя принятые и отклонённые формы и сохраняя зелёную native-сборку.

Цель `99%` не разрешает оптимизировать только среднее. В итоговом отчёте обязательно показать невзвешенное среднее, среднее с весом по числу инструкций, strict fuzzy и результаты каждой крупной функции отдельно.

## Целевые символы

- `AbyssEngine::PaintCanvas::TextureCreate(unsigned short, unsigned int&, bool)` — сохранить существующее linked-exact совпадение.
- callback-вариант `PaintCanvas::TextureCreate`.
- `PaintCanvas::TextureCreateGlobal`.
- `AbyssEngine::TextureCreateFromFile`.
- `AbyssEngine::TextureCreateFromFileIntern`.
- Restore/cube/loader-варианты включать только если Android-таблица вызовов доказывает принадлежность к тому же семейству.

## Задания

1. Зафиксировать baseline: native build, текущую ARM-компиляцию, focused-отчёт семейства и полный corpus.
2. Устранить три текущих ARM compile failure минимальными типовыми исправлениями. Не переписывать игровое поведение ради компиляции.
3. Составить точный список TextureCreate-символов, адресов Android и текущих метрик.
4. Восстанавливать по одному независимому телу или связанному ownership-пакету: заголовок AEI, callback, allocator/cleanup, material attachment и error path.
5. После каждого принятого пакета запускать focused verify. После изменения общих заголовков или ABI запускать полный corpus.
6. Сохранять rejected trials с причиной отклонения и метриками; исходник после отклонённой формы возвращать к принятому состоянию.
7. В финале обновить отдельный документ семейства TextureCreate и `docs/PROJECT_STATUS.md`/`docs/VALIDATION.md` только подтверждёнными результатами.

## Источники истины

1. Android ARM32 `libgof2hdaa.so` в локальном `_work/bins` и таблицы `_work/symbols`.
2. `C:\Users\Dima\Documents\Galaxy on Fire new version\analysis\gof2_libgof2hdaa_full_ida.c`.
3. iOS IDA/Ghidra дампы в основном workspace — только как перекрёстная проверка ABI и семантики.
4. PC-код/ресурсы — только как версионная проверка, не как доказательство Android ARM shape.

## Обязательные проверки

- UCRT64 native build `gof2` проходит.
- ARM current-source coverage равен `204/204` либо оставшийся внешний блокер доказан и документирован.
- Focused TextureCreate report сохраняется в `_work/` и кратко переносится в документ результата.
- Полный ARM corpus не теряет уже существующие linked-exact/byte-exact функции без доказанной семантической причины.
- Ни один принятый патч не использует искусственные stack locals, volatile/register barriers, фиктивные ветки или мёртвый код ради процента.

## Граница ветки

Не изменять `MeshReadData`, `MeshCreateFromFile` и `Mesh::ReadEnhancedDataFromFile`: ими владеет ветка Gemini. Если TextureCreate требует их изменения, зафиксировать зависимость в отчёте и остановиться на границе.

Если подтверждённый исходниками корректный вариант не достигает `99%`, сохранить корректный вариант и честно описать метрический потолок. Не ухудшать семантику ради цели.
