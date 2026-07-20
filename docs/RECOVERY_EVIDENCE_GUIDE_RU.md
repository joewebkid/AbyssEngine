# Руководство По Источникам Для Восстановления

Дата: 2026-07-20

## Назначение

Этот документ задает порядок работы с разными версиями *Galaxy on Fire* и
Abyss Engine. Целевой продукт репозитория - Android ARMv7 версия GOF2 HD.
Другие версии полезны для смысла, таблиц и архитектуры, но не могут молча
заменить Android ARM как источник истины.

## Быстрый Выбор Источника

| Задача | Основной источник | Нужная проверка | Что он не доказывает |
| --- | --- | --- | --- |
| Сигнатура, 32-bit layout, vtable, ctor/dtor, byte-match | Android 2.0.16 `libgof2hdaa.so`, symbol/thumb maps, ARM disassembly | Android IDA и Ghidra, focused ARM verify | Java, iOS и PC не доказывают Android offsets или bytes. |
| Тело Android функции | Android IDA и Ghidra dumps с известным symbol/address | Сверить CFG, calls и constants между двумя dumps; при споре смотреть ARM | Один decompiler dump не безошибочный source. |
| Игровая логика и state machine | Android body | GOF2 J2ME или DeepOpen только для смысла | J2ME не доказывает masks, ownership, offsets или exact flow HD. |
| UI, тачи, строки | Android `MenuTouchWindow`, `MGame`, `Hud` | iOS/J2ME, GameText IDs, image IDs и native touch order | J2ME coordinates и UI tree не равны HD UI. |
| AEM/AEI, material/resource chain | Android native loader и реальные header/payload samples | iOS loader, decoded samples и resource-table linkage | Один decoded asset не подтверждает все compression/FX states. |
| Шипы, оружие, HUD, combat | Android class body и data tables | iOS/J2ME intent плюс runtime/resource evidence | Нельзя переносить Java formula без Android anchor. |
| GOF1 контент и ранние алгоритмы | GOF1 source/tables | Найти соответствие в GOF2 HD, если это влияет на C++ | GOF1 не доказывает GOF2 HD behavior. |
| Общие Abyss Engine patterns | Alliances, GOF3, Star Battalion | Подтвердить GOF2 Android call site/layout | Похожее имя класса не доказывает ABI. |
| Godot port | Подтвержденное native evidence | Godot-native implementation и отдельная маркировка heuristic | Godot code не становится recovered C++ и не участвует в ARM match. |

## Локальная Карта Материалов

Все эти наборы находятся вне Git-дерева decomp. Оригинальные binaries,
APK/IPA/JAR и extracted assets не коммитятся.

| Набор | Обычное расположение | Роль |
| --- | --- | --- |
| Android GOF2 HD | `_work/bins/android_2.0.16_libgof2hdaa.so`, `_work/symbols/` | Единственный источник ARM layout и byte-match. |
| Android IDA | `../analysis/gof2_libgof2hdaa_full_ida.c`, `../analysis/gof2_ida_smart_dump.c` | Поиск symbols, calls, literals и high-level CFG. |
| Android Ghidra | `../analysis/gof2_gidra_engine_full_source.c` | Независимая проверка CFG, локальных объектов и спорных мест. |
| iOS GOF2 | `../analysis/gof2_ios_ida_full_dump.c`, `../analysis/gof2_gidra_ios_engine_full_source.c`, `../references/GOF2 IOS/` | Alternate semantic/source cross-check; ABI другой. |
| GOF2 J2ME | `../references/Galaxy_on_Fire_2_v1_0_3_decompiled/` и JAR archives | Читаемые алгоритмы, menu intent, content/string meaning. |
| DeepOpen | `../references/DeepOpen/` at pinned `d300f93` | Внешняя open J2ME crosswalk, не dependency и не ARM proof. |
| GOF1 | `../analysis/gof1_engine_full_source.c`, `../analysis/gof1_decoded/`, GOF1 archives | Предшествующая кампания, таблицы и ранний engine intent. |
| Alliances | `../analysis/gof_alliances_full_ida.c`, `../analysis/alliances_*` probes, iOS archive | Форматы и engine patterns только после GOF2 cross-check. |
| GOF3 / Star Battalion | `../analysis/gof3_*`, `../analysis/star_battalion_*`, shared archives | Дальняя comparative база для engine API, physics и resources. |
| Reforged, mods, видео | Shared references и captures | Имена, content leads, usability tests; никогда не ground truth. |

Публичные ссылки на переносимые исследовательские архивы находятся в
[`README.md`](../README.md#supplementary-source-archives). Они не заменяют
локальный reference binary.

## Иерархия Доказательств

1. **Android ARM instruction/symbol/data**: ABI, layout, call order, constants
   и byte-match.
2. **Совпадающие Android IDA и Ghidra**: понятное тело при известной привязке
   к ARM адресу.
3. **iOS GOF2, подтверждающий Android path**: имя, ownership или intent без
   переноса offsets.
4. **GOF2 J2ME/DeepOpen, подтверждающий Android path**: алгоритм или state
   machine без ARM-claim.
5. **GOF1, Alliances, GOF3, Star Battalion**: lead или hypothesis, пока нет
   GOF2 Android evidence.
6. **Community ports, mods, video и Godot code**: reference для ощущения и
   content, не proof native behavior.

Статусы для кода и документации:

- `source-backed`: есть named native/iOS/Java/symbol/disassembly anchor;
- `confirmed`: есть независимая вторая опора или matching result;
- `needs confirmation`: связь правдоподобна, но недостаточно доказана;
- `heuristic`: практическое решение без заявления об оригинале.

## Процесс Для Одного Пакета

### 1. Сформулировать узкую цель

Подходящие цели: `MGame::OnTouchEnd` selector `+0xca`, `Radar::draw`
enemy-loop, AEI ETC1 routing. Неподходящая цель: "восстановить весь MGame".

Для цели сразу зафиксировать:

- Android symbol/address, если известен;
- C++ files и offsets;
- ожидаемый результат: source-backed body, typed layout, importer behavior или
  ARM source shape.

### 2. Собрать Evidence Packet До Редактирования

Для Android функции минимум требуются:

1. IDA excerpt с symbol/address.
2. Соответствующий Ghidra excerpt.
3. Текущий C++ body/header и call sites.
4. Secondary source только если он разрешает неоднозначность.
5. Отдельные списки `confirmed` и `needs confirmation`.

Если address/symbol отсутствует, сначала найти anchor через symbol map, xref,
vtable или data reference. Не подменять отсутствующую Android функцию Java body.

### 3. Выбрать Способ Переноса

| Находка | Правильное действие |
| --- | --- |
| Одна ветка с ясными offsets/calls | Восстановить в существующем классе; неизвестные поля оставить `field_0xNN`. |
| Constructor/vtable | Сначала header, allocation size, ctor variants и virtual order; потом body. |
| Большая byte-match функция | Переносить coherent blocks в parent body в native order; helper/shim оставлять только пока контракт не доказан. |
| String/formatting | Зафиксировать GameText ID, tokens и lifetime `String`; не переводить literal вручную. |
| AEM/AEI resource | Проследить `resource ID -> loader -> payload -> material/mesh consumer`; unknown render states оставить в sidecar. |
| Godot behavior | Перенести наблюдаемую семантику Godot-native средствами, а пробелы пометить `heuristic`. |

Radare2/rizin допустимы для xref, block boundaries и literals, когда IDA/Ghidra
расходятся. Они не повышают статус находки без подтверждения GOF2 Android.

### 4. Валидация

После C++ изменения:

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt --target gof2 -- -k 0
```

Для byte-match-пакета дополнительно запускать focused ARM compare:

```powershell
$env:GOF2_VERIFY_LOCAL_NDK = '1'
$env:GOF2_NDK_ROOT = (Resolve-Path '.cache\ndk\android-ndk-r18b')
C:\msys64\ucrt64\bin\python.exe tools\verify\verify.py `
  --build-dir cmake-build-match\verify --no-build `
  --unit game/menu/MGame --show _ZN5MGame10OnTouchEndEiiPv
```

Записывать before/after fuzzy percentage, instruction counts, `linked_equal` и
`bytes_equal`. Успешная x64/UCRT build не является ARM proof. Небольшой fuzzy
regression не требует отката, если исправление прямо подтверждено Android body.

### 5. Документация И Коммит

- создать или обновить focused note в `docs/`;
- обновить `docs/PROJECT_STATUS.md`, если изменился статус/metric;
- перечислить risks и unknown fields;
- выполнить `git diff --check`;
- коммитить только код, docs и tooling текущего пакета.

## Частые Ошибки

- Называть соседние байты одним флагом только из удобства.
- Добавлять defensive null-check в match path без Android ветки.
- Делать массовый asset import без доказанной resource chain.
- Перетаскивать Java coordinates, masks или pointer ownership в Android HD.
- Коммитить original binaries, assets, APK/IPA/JAR, NDK, build output или
  external reference trees.

## Шаблон Нового Findings Note

```md
### <symbol or package>

- Target: Android 2.0.16 `<symbol>` at `<address>`.
- Primary evidence: `<IDA dump>` and `<Ghidra dump>`.
- Secondary evidence: `<iOS/J2ME/other>` or `none`.
- Confirmed: `<calls, constants, offsets, branches>`.
- Needs confirmation: `<fields, ownership, missing branch>`.
- Validation: `<native build>`, `<focused ARM result>`.
- Claim: `source-backed` / `confirmed` / `needs confirmation` / `heuristic`.
```

Так все версии игры остаются полезными, но не смешиваются в одну
несуществующую "истину".
