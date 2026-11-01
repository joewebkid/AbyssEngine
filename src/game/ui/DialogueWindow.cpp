#include "game/ui/DialogueWindow.h"
#include "game/ui/ChoiceWindow.h"
#include "engine/audio/FModSound.h"
#include "game/ui/ScrollTouchWindow.h"
#include "game/mission/Status.h"
#include "game/ship/Agent.h"
#include "engine/core/GameText.h"
#include "engine/render/ImageFactory.h"
#include "engine/render/ImagePart.h"
#include "game/ui/Layout.h"
#include "engine/render/PaintCanvas.h"
#include "engine/core/AERandom.h"
#include "game/mission/Mission.h"
#include "game/world/Standing.h"
#include "game/world/Level.h"
#include "game/core/String.h"
#include "game/core/Globals.h"
#include "game/ui/TouchButton.h"


int DialogueWindow::OnTouchBegin(int x, int y) {
    if (this->choiceActive != 0) {
        this->choiceWindow->OnTouchBegin(x, y);
    } else {
        this->scrollWindow->OnTouchBegin(x, y);
        this->prevButton->OnTouchBegin(x, y);
        this->nextButton->OnTouchBegin(x, y);
        this->moreButton->OnTouchBegin(x, y);
    }
    return 0;
}


static const int g_dw_briefingCounts[0xa2] = {
    4, 0, 10, 0, 2, 0, 0, 8, 0, 0, 0, 2,
    0, 0, 2, 0, 4, 0, 0, 0, 0, 4, 0, 2,
    8, 0, 6, 0, 0, 2, 0, 0, 2, 0, 0, 0,
    4, 0, 2, 0, 0, 6, 0, 0, 0, 0, 0, 0,
    14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 22, 0, 8, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0
};

static const int g_dw_successCounts[0xa2] = {
    0, 38, 6, 10, 4, 12, 2, 10, 18, 22, 24, 26,
    16, 4, 0, 22, 6, 34, 16, 10, 2, 2, 20, 28,
    0, 6, 4, 22, 0, 2, 10, 20, 22, 18, 30, 20,
    4, 16, 4, 20, 0, 10, 8, 4, 8, 0, 20, 32,
    16, 0, 0, 0, 0, 0, 16, 28, 4, 20, 24, 2,
    16, 16, 26, 4, 10, 34, 24, 16, 20, 0, 2, 14,
    30, 18, 4, 26, 20, 40, 0, 0, 12, 0, 16, 18,
    0, 4, 8, 0, 16, 0, 18, 0, 14, 22, 14, 32,
    20, 6, 24, 24, 0, 20, 12, 20, 14, 0, 0, 0,
    30, 8, 6, 10, 6, 10, 2, 8, 36, 12, 2, 22,
    0, 18, 36, 0, 30, 12, 12, 22, 8, 0, 18, 0,
    14, 6, 16, 4, 12, 0, 48, 2, 24, 20, 10, 12,
    0, 2, 24, 16, 12, 10, 6, 16, 16, 22, 32, 8,
    10, 0, 10, 22, 6, 12
};

int DialogueWindow::getMode() {
    return this->kind;
}

int DialogueWindow::length() {
    Mission *mission = this->mission;
    if (mission != 0 && mission->isCampaignMission() != 0) {
        const int *counts;
        if (this->kind == 1) {
            counts = g_dw_successCounts;
        } else {
            if (this->kind != 0) return 1;
            counts = g_dw_briefingCounts;
        }
        return counts[this->campaignMission] / 2;
    }
    if (this->kind == 0 && this->mission != 0 &&
        this->mission->getTargetStation() == 0x6c) {
        int result = 6;
        if (Globals::status->field_114 == 2) result = 0x12;
        return result;
    }
    return 1;
}

int DialogueWindow::nextPage() {
    int len = this->length();
    if (this->page < len - 1) {
        this->page = this->page + 1;
        this->loadContent();
        return 1;
    }
    return 0;
}

bool DialogueWindow::hasLevel() {
    return this->level != 0;
}

DialogueWindow::~DialogueWindow() {
    if (this->faceParts != 0) {
        ArrayReleaseClasses(*this->faceParts); ArrayRemoveAll(*(this->faceParts));
        delete this->faceParts;
        this->faceParts = 0;
    }

    delete[] this->briefingOffsets;
    this->briefingOffsets = 0;
    delete[] this->successOffsets;
    this->successOffsets = 0;

    delete this->scrollWindow;
    this->scrollWindow = 0;

    delete this->prevButton;
    this->prevButton = 0;
    delete this->nextButton;
    this->nextButton = 0;
    delete this->moreButton;
    this->moreButton = 0;

    { if (this->agentName.data) delete[] this->agentName.data; this->agentName.data = nullptr; this->agentName.length = 0; }
    { if (this->bodyText.data) delete[] this->bodyText.data; this->bodyText.data = nullptr; this->bodyText.length = 0; }
}

int DialogueWindow::OnTouchMove(int x, int y) {
    if (this->choiceActive != 0) {
        this->choiceWindow->OnTouchMove(x, y);
    } else {
        this->scrollWindow->OnTouchMove(x, y);
        this->prevButton->OnTouchMove(x, y);
        this->nextButton->OnTouchMove(x, y);
        this->moreButton->OnTouchMove(x, y);
    }
    return 0;
}

bool DialogueWindow::isFirstPage() {
    return this->page == 0;
}

int DialogueWindow::previousPage() {
    if (this->page <= 0) return 0;
    this->page = this->page - 1;
    this->loadContent();
    return 1;
}

bool DialogueWindow::isLastPage() {
    int len = this->length();
    return this->page == len - 1;
}


bool DialogueWindow::hasBriefingDialogue(int id) {
    if (id > 0xa1) return false;
    return g_dw_briefingCounts[id] > 0;
}


bool DialogueWindow::hasSuccessDialogue(int id) {
    if (id > 0xa1) return false;
    return g_dw_successCounts[id] > 0;
}

void DialogueWindow::set(Mission *mission, int kind, int campaign) {
    this->mission = mission;
    this->kind = kind;
    if (kind == 1) {
        mission->getAgent();
        mission->setWon(true);
    } else if (kind == 2) {
        Agent *agent = mission->getAgent();
        if (agent != 0 && agent->isGenericAgent() == 0) {
            agent->setOfferAccepted(false);
        }
        mission->setFailed(true);
    }

    this->page = 0;
    if (campaign == -1) {
        campaign = Status::gStatus->getCurrentCampaignMission();
    }
    this->campaignMission = campaign;
    this->loadContent();
}


static const char g_dw_emptyLoad[] = "";

static const int g_dw_genericTextIds[15] = {
    379, 380, 381, 382, 383,
    373, 374, 375, 376, 377,
    384, 385, 386, 387, 388
};

static const int g_dw_specialStationStatus2[36] = {
    4, 459, 0, 460, 4, 461, 0, 462, 4, 463, 0, 464,
    4, 465, 0, 466, 4, 467, 0, 468, 4, 469, 0, 470,
    4, 471, 0, 472, 4, 473, 0, 474, 4, 475, 16, 476
};

static const int g_dw_specialStationDefault[12] = {
    4, 479, 0, 480, 4, 481, 0, 482, 4, 483, 0, 484
};

static const int g_dw_campaignBriefingRows[120] = {
    16, 1677, 16, 1680, 0, 1721, 2, 1722,
    0, 1723, 2, 1724, 16, 1725, 2, 1738,
    2, 1748, 16, 1749, 16, 1750, 16, 1753,
    0, 1795, 0, 1819, 1, 1836, 0, 1837,
    1, 1877, 16, 1878, 5, 1897, 6, 1913,
    0, 1914, 6, 1915, 0, 1916, 6, 1928,
    0, 1929, 6, 1930, 0, 1947, 0, 1970,
    7, 2016, 0, 2017, 0, 2028, 0, 2049,
    7, 2050, 0, 2051, 0, 2105, 24, 2106,
    0, 2107, 24, 2108, 0, 2109, 24, 2110,
    0, 2111, 0, 2240, 20, 2241, 0, 2242,
    20, 2243, 0, 2244, 20, 2245, 0, 2246,
    26, 2247, 0, 2248, 26, 2249, 0, 2250,
    31, 2283, 0, 2284, 31, 2285, 0, 2286,
    49, 2879, 0, 2880, 49, 2881, 0, 2882
};

static const int g_dw_campaignSuccessRows1To123[1562] = {
    0, 1700, 2, 1701, 0, 1702, 2, 1703,
    0, 1704, 2, 1705, 0, 1706, 2, 1707,
    2, 1708, 0, 1709, 2, 1710, 2, 1711,
    0, 1712, 2, 1713, 0, 1714, 2, 1715,
    2, 1716, 0, 1717, 16, 1718, 2, 1728,
    0, 1729, 16, 1730, 2, 1733, 0, 1734,
    2, 1735, 0, 1736, 2, 1737, 0, 1739,
    2, 1740, 2, 1741, 0, 1742, 2, 1743,
    0, 1744, 2, 1745, 16, 1746, 2, 1747,
    2, 1758, 0, 1759, 2, 1760, 0, 1761,
    2, 1762,
    2, 1763, 0, 1764, 2, 1765, 0, 1766,
    2, 1767, 0, 1768, 2, 1769, 0, 1770,
    2, 1771, 17, 1772, 17, 1773, 17, 1774,
    2, 1775, 0, 1776, 2, 1777, 0, 1778,
    2, 1779, 0, 1780, 2, 1781, 16, 1782,
    3, 1783, 0, 1784, 3, 1785, 0, 1786,
    3, 1787, 0, 1788, 3, 1789, 0, 1790,
    3, 1791, 0, 1792, 3, 1793, 0, 1794,
    0, 1796, 0, 1797, 4, 1798, 0, 1799,
    4, 1800, 0, 1801, 4, 1802, 0, 1803,
    4, 1804, 0, 1805, 0, 1806, 4, 1807,
    0, 1808, 3, 1809, 0, 1810, 3, 1811,
    0, 1812, 3, 1813, 0, 1814, 3, 1815,
    16, 1816, 3, 1817, 0, 1818,
    17, 1825, 1, 1826, 0, 1827, 1, 1828,
    0, 1829, 1, 1830, 0, 1831, 1, 1832,
    12, 1833, 0, 1834, 1, 1835,
    1, 1843, 0, 1844, 1, 1845,
    1, 1846, 0, 1847, 1, 1848, 0, 1849,
    1, 1850, 0, 1851, 1, 1852, 1, 1853,
    0, 1854, 1, 1855, 0, 1856, 1, 1857,
    0, 1858, 1, 1859, 0, 1860, 1, 1861,
    16, 1862,
    0, 1863, 13, 1864, 0, 1865, 13, 1866,
    0, 1867, 1, 1868, 0, 1869, 1, 1870,
    1, 1871, 0, 1872, 1, 1873, 0, 1874,
    1, 1875,
    0, 1876,
    1, 1886,
    5, 1887, 0, 1888, 5, 1889, 5, 1890,
    0, 1891, 5, 1892, 0, 1893, 0, 1894,
    5, 1895, 16, 1896,
    6, 1898, 0, 1899, 6, 1900, 0, 1901,
    6, 1902, 0, 1903, 6, 1904, 0, 1905,
    6, 1906, 0, 1907, 6, 1908, 0, 1909,
    16, 191, 16, 192,
    6, 1922, 0, 1923, 6, 1924,
    6, 1931, 0, 1932,
    6, 1933, 0, 1934, 17, 1935, 6, 1936,
    0, 1937, 6, 1938, 0, 1939, 6, 1940,
    0, 1941, 6, 1942, 0, 1943,
    0, 1954,
    0, 1955, 6, 1956, 0, 1957, 6, 1958,
    0, 1959,
    1, 1960, 0, 1961, 1, 1962, 0, 1963,
    1, 1964, 0, 1965, 1, 1966, 0, 1967,
    1, 1968, 0, 1969,
    20, 1971, 0, 1972, 20, 1973, 0, 1974,
    20, 1975, 0, 1976, 20, 1977, 0, 1978,
    20, 1979, 20, 1980, 0, 1981,
    20, 1982, 0, 1983, 0, 1984, 6, 1985,
    0, 1986, 6, 1987, 0, 1988, 6, 1989,
    0, 1990,
    1, 1991, 0, 1992, 1, 1993, 0, 1994,
    1, 1995, 0, 1996, 1, 1997, 0, 1998,
    1, 1999, 0, 2000, 1, 2001, 0, 2002,
    1, 2003, 0, 2004, 1, 2005,
    7, 2006, 0, 2007, 7, 2008, 0, 2009,
    7, 2010, 0, 2011, 7, 2012, 0, 2013,
    7, 2014, 0, 2015,
    7, 2018, 0, 2019,
    1, 2020, 0, 2021, 1, 2022, 0, 2023,
    1, 2024, 0, 2025, 1, 2026, 0, 2027,
    0, 2030, 1, 2031,
    1, 2032, 0, 2033, 1, 2034, 0, 2035,
    1, 2036, 0, 2037, 1, 2038, 1, 2039,
    1, 2040, 0, 2041,
    0, 2058, 7, 2059, 0, 2060, 7, 2061,
    0, 2062,
    0, 2065, 0, 2066, 6, 2067, 0, 2068,
    0, 2069, 6, 2070,
    0, 2075, 0, 2076, 0, 2077, 0, 2078,
    15, 2079, 0, 2080, 23, 2081, 0, 2082,
    23, 2083, 0, 2084, 36, 2085, 0, 2086,
    36, 2087, 0, 2088,
    36, 2089, 0, 2090, 36, 2091, 0, 2092,
    36, 2093, 0, 2094, 36, 2095, 0, 2096,
    36, 2097, 0, 2098, 36, 2099, 0, 2100,
    36, 2101, 0, 2102, 36, 2103, 0, 2104,
    25, 2112, 0, 2113, 25, 2114, 0, 2115,
    25, 2116, 0, 2117, 25, 2118, 0, 2119,
    23, 2130, 0, 2131, 23, 2132, 0, 2133,
    23, 2134, 0, 2135, 23, 2136, 0, 2137,
    26, 2140, 0, 2141, 26, 2142, 0, 2143,
    26, 2144, 0, 2145, 26, 2146, 0, 2147,
    26, 2148, 0, 2149, 26, 2150, 0, 2151,
    26, 2152, 0, 2153,
    27, 2160, 0, 2161,
    26, 2162, 0, 2163, 26, 2164, 0, 2165,
    26, 2166, 0, 2167, 26, 2168, 0, 2169,
    26, 2170, 16, 2171,
    0, 2173, 26, 2174, 0, 2175, 26, 2176,
    0, 2177, 26, 2178, 0, 2179, 26, 2180,
    0, 2181, 26, 2182, 0, 2183, 16, 2184,
    0, 2190,
    0, 2193, 26, 2194, 0, 2195, 26, 2196,
    0, 2197, 26, 2198, 0, 2199, 0, 2200,
    15, 2201, 0, 2202, 6, 2203, 0, 2204,
    6, 2205, 0, 2206, 6, 2207, 0, 2208,
    6, 2210, 0, 2211, 6, 2212, 0, 2213,
    6, 2214, 0, 2215, 6, 2216, 0, 2217,
    6, 2218, 0, 2219, 6, 2220, 0, 2221,
    6, 2222,
    28, 2226, 0, 2227,
    0, 2235, 20, 2236, 0, 2237, 20, 2238,
    0, 2239, 6, 2254, 0, 2255, 6, 2256,
    0, 2257, 6, 2258, 0, 2259, 6, 2260,
    0, 2261, 6, 2262, 0, 2263, 6, 2264,
    0, 2265, 6, 2266, 0, 2267, 6, 2268,
    0, 2269, 0, 2270, 26, 2271, 0, 2272,
    26, 2273, 0, 2274, 26, 2275, 0, 2276,
    26, 2277, 0, 2278, 26, 2279, 31, 2280,
    0, 2281, 31, 2282, 0, 2299, 20, 2300,
    0, 2301, 20, 2302, 0, 2303, 20, 2304,
    0, 2305, 0, 2306, 32, 2308, 0, 2309,
    32, 2310, 0, 2311, 32, 2312, 0, 2313,
    32, 2314, 0, 2315, 32, 2316, 0, 2317,
    0, 2324, 32, 2325, 0, 2326, 32, 2327,
    0, 2328, 32, 2329, 0, 2330, 32, 2331,
    0, 2332, 26, 2333, 0, 2334, 26, 2335,
    0, 2336, 26, 2337, 0, 2338, 26, 2339,
    0, 2340, 26, 2341, 0, 2342, 26, 2343,
    0, 2344, 26, 2345, 0, 2346, 0, 2355,
    33, 2356, 0, 2357, 33, 2358, 0, 2359,
    33, 2360, 0, 2361, 33, 2362, 0, 2363,
    6, 2364, 0, 2365, 0, 2366, 6, 2367,
    0, 2368, 6, 2369, 0, 2370, 6, 2371,
    0, 2372, 6, 2373, 0, 2374, 6, 2375,
    0, 2376, 6, 2377, 0, 2378, 20, 2379,
    0, 2380, 20, 2381, 0, 2382, 20, 2383,
    0, 2384, 20, 2385, 0, 2386, 20, 2387,
    0, 2388, 26, 2389, 0, 2390, 26, 2391,
    0, 2392, 26, 2393, 0, 2394, 26, 2395,
    0, 2396, 26, 2397, 31, 2398, 26, 2399,
    31, 2400, 0, 2401, 26, 2402, 0, 2403,
    26, 2404, 0, 2405, 26, 2406, 0, 2407,
    26, 2408, 0, 2426, 6, 2427, 0, 2428,
    6, 2429, 0, 2430, 6, 2431, 6, 2436,
    0, 2437, 6, 2438, 0, 2439, 6, 2440,
    0, 2441, 6, 2442, 0, 2443, 0, 2444,
    0, 2445, 0, 2446, 0, 2447, 0, 2448,
    20, 2449, 0, 2450, 20, 2451, 0, 2452,
    6, 2474, 0, 2475, 6, 2476, 0, 2477,
    6, 2478, 0, 2479, 17, 2486, 0, 2487,
    6, 2488, 0, 2489, 6, 2490, 0, 2491,
    6, 2492, 0, 2493, 2, 2495, 0, 2496,
    2, 2497, 0, 2498, 2, 2499, 0, 2500,
    2, 2501, 0, 2502, 2, 2503, 0, 2523,
    2, 2524, 0, 2525, 2, 2526, 0, 2527,
    2, 2528, 0, 2529, 37, 2530, 0, 2531,
    0, 2532, 37, 2533, 0, 2534, 37, 2535,
    37, 2536, 0, 2537, 37, 2538, 37, 2539,
    0, 2540, 0, 2548, 37, 2549, 0, 2550,
    37, 2551, 0, 2552, 37, 2553, 0, 2554,
    6, 2556, 15, 2557, 6, 2558, 38, 2559,
    6, 2560, 38, 2561, 6, 2562, 38, 2563,
    40, 2564, 6, 2565, 20, 2566, 38, 2567,
    20, 2568, 38, 2569, 40, 2570, 6, 2571,
    0, 2572, 1, 2573, 0, 2574, 1, 2575,
    0, 2576, 1, 2577, 0, 2578, 1, 2579,
    0, 2580, 1, 2581, 0, 2585, 22, 2586,
    0, 2587, 39, 2588, 0, 2589, 39, 2590,
    0, 2591, 39, 2592, 0, 2593, 39, 2594,
    0, 2595, 39, 2596, 0, 2597, 39, 2598,
    0, 2599, 38, 2601, 6, 2602, 38, 2603,
    6, 2604, 38, 2605, 6, 2606, 38, 2607,
    6, 2608, 38, 2609, 6, 2610, 38, 2611,
    6, 2612, 1, 2615, 0, 2616, 1, 2617,
    0, 2618, 1, 2619, 0, 2620, 1, 2621,
    0, 2622, 1, 2623, 0, 2624, 6, 2634,
    0, 2635, 6, 2636, 0, 2637, 6, 2638,
    0, 2639, 6, 2640, 20, 2641, 20, 2642,
    20, 2643, 0, 2644, 20, 2645, 20, 2646,
    0, 2647, 20, 2648, 0, 2649,
    20, 2650, 20, 2651, 0, 2652, 20, 2653,
    0, 2654, 6, 2655, 0, 2656, 6, 2672,
    0, 2673, 6, 2674, 0, 2675, 6, 2676,
    0, 2677, 6, 2678, 0, 2679, 6, 2680,
    0, 2681, 6, 2682, 0, 2683, 6, 2684,
    0, 2685, 6, 2686, 37, 2688, 1, 2689,
    37, 2690, 1, 2691, 0, 2692, 42, 2693,
    0, 2694, 0, 2695, 41, 2696, 0, 2697,
    41, 2698, 0, 2699, 0, 2700, 41, 2701,
    0, 2702, 0, 2703, 62, 2704, 0, 2705,
    62, 2706, 0, 2707,
    0, 2711, 0, 2712, 62, 2713, 0, 2714,
    0, 2715, 38, 2720, 0, 2721, 38, 2722,
    0, 2723, 40, 2724, 0, 2725, 4, 2726,
    0, 2727, 4, 2728, 0, 2729, 4, 2730,
    0, 2731, 4, 2732, 0, 2733, 4, 2734,
    0, 2735, 4, 2736, 0, 2737, 0, 2738,
    43, 2739, 0, 2740, 43, 2741, 0, 2742,
    0, 2743, 0, 2744, 6, 2746, 20, 2747,
    6, 2748, 20, 2749, 6, 2750, 20, 2751,
    6, 2752, 20, 2753, 6, 2754, 20, 2755,
    6, 2756, 0, 2758, 4, 2759, 38, 2760,
    0, 2761, 38, 2762, 40, 2763, 0, 2764,
    4, 2765, 0, 2766, 6, 2767, 38, 2768,
    6, 2769, 38, 2770, 0, 2771, 38, 2772,
    0, 2773, 38, 2774, 20, 2775, 38, 2776,
    38, 2777, 6, 2778, 38, 2779, 0, 2780,
    38, 2781, 6, 2782, 38, 2783, 0, 2784
};

static int g_dw_campaignSuccessTextIds[0x1000];

static int g_dw_defaultClientImage[5];
static int g_dw_stationPortrait1[5] = {0, 8, 8, 1, 8};
static int g_dw_stationPortrait2[5] = {2, 1, 1, 1, 1};
static int g_dw_stationPortrait3[5] = {0, 9, 9, 8, 9};
static int g_dw_stationPortrait5[5] = {0, 2, 3, 3, 10};
static int g_dw_stationPortrait6[5] = {10, 0, 0, 1, 0};
static int g_dw_stationPortrait7[5] = {1, 1, 0, 4, 0};
static int g_dw_stationPortrait20[5] = {7, 1, 1, 0, 0};
static int g_dw_stationPortrait24[5] = {1, 2, 4, 3, 1};
static int g_dw_stationPortrait26[5] = {10, 3, 3, 4, 6};
static int g_dw_stationPortrait31[5] = {0, 2, 10, 9, 2};
static int g_dw_stationPortrait49[5] = {2, 1, 0, 4, 0};
static int g_dw_stationPortrait4[5] = {4, 2, 0, 2, 2};
static int g_dw_stationPortrait16[5] = {11, 1, 0, 0, 2};
static int g_dw_stationPortrait17[5] = {11, 1, 0, 0, 3};
static int g_dw_stationPortrait12[5] = {0, 5, 4, 2, 7};
static int g_dw_stationPortrait13[5] = {10, 1, 1, 2, 4};
static int g_dw_stationPortrait15[5] = {11, 1, 0, 0, 1};
static int g_dw_stationPortrait23[5] = {0, 2, 10, 9, 2};
static int g_dw_stationPortrait25[5] = {1, 3, 1, 5, 0};
static int g_dw_stationPortrait36[5] = {0, 2, 10, 9, 2};
static int g_dw_stationPortrait27[5] = {0, 0, 5, 8, 6};
static int g_dw_stationPortrait28[5] = {0, 8, 8, 8, 0};
static int g_dw_stationPortrait32[5] = {7, 0, 0, 2, 1};
static int g_dw_stationPortrait33[5] = {0, 5, 4, 8, 3};
static int g_dw_stationPortrait22[5] = {2, 0, 0, 0, 0};
static int g_dw_stationPortrait37[5] = {2, 4, 1, 4, 1};
static int g_dw_stationPortrait38[5] = {0, 8, 3, 8, 3};
static int g_dw_stationPortrait39[5] = {2, 2, 1, 3, 2};
static int g_dw_stationPortrait40[5] = {6, 0, 0, 2, 0};
static int g_dw_stationPortrait8[5] = {0, 6, 8, 8, 7};
static int g_dw_stationPortrait9[5] = {0, 4, 8, 8, 10};
static int g_dw_stationPortrait10[5] = {0, 3, 5, 8, 6};
static int g_dw_stationPortrait11[5] = {0, 7, 7, 8, 5};
static int g_dw_stationPortrait14[5] = {2, 2, 2, 4, 2};
static int g_dw_stationPortrait18[5] = {0, 8, 10, 10, 4};
static int g_dw_stationPortrait19[5] = {9, 0, 0, 0, 0};
static int g_dw_stationPortrait21[5] = {2, 0, 0, 1, 2};
static int g_dw_stationPortrait29[5] = {0, 4, 8, 8, 4};
static int g_dw_stationPortrait30[5] = {0, 0, 5, 8, 10};
static int g_dw_stationPortrait34[5] = {4, 1, 1, 1, 1};
static int g_dw_stationPortrait35[5] = {0, 4, 8, 8, 4};
static int g_dw_stationPortrait41[5] = {10, 3, 3, 1, 4};
static int g_dw_stationPortrait42[5] = {0, 0, 5, 4, 6};
static int g_dw_stationPortrait43[5] = {1, 0, 3, 1, 2};
static int g_dw_stationPortrait44[5] = {2, 4, 1, 2, 3};
static int g_dw_stationPortrait45[5] = {1, 0, 1, 1, 3};
static int g_dw_stationPortrait46[5] = {7, 0, 0, 0, 0};
static int g_dw_stationPortrait47[5] = {2, 4, 2, 0, 0};
static int g_dw_stationPortrait48[5] = {2, 0, 1, 1, 2};
static int g_dw_stationPortrait50[5] = {1, 0, 0, 3, 5};
static int g_dw_stationPortrait51[5] = {0, 3, 3, 0, 5};
static int g_dw_stationPortrait52[5] = {10, 0, 3, 3, 5};
static int g_dw_stationPortrait53[5] = {0, 3, 6, 0, 8};
static int g_dw_stationPortrait54[5] = {10, 3, 0, 3, 3};
static int g_dw_stationPortrait55[5] = {0, 10, 7, 2, 1};
static int g_dw_stationPortrait56[5] = {12, 0, 0, 0, 0};
static int g_dw_stationPortrait57[5] = {0, 0, 10, 0, 7};
static int g_dw_stationPortrait58[5] = {0, 6, 0, 7, 4};
static int g_dw_stationPortrait59[5] = {2, 3, 1, 1, 4};
static int g_dw_stationPortrait60[5] = {2, 1, 4, 2, 0};
static int g_dw_stationPortrait61[5] = {0, 0, 4, 3, 5};
static int g_dw_stationPortrait62[5] = {0, 10, 2, 9, 4};
// Portrait indices used by the station and campaign dialogue tables.
static int *const g_dw_stationPortraits[63] = {
    g_dw_defaultClientImage, g_dw_stationPortrait1, g_dw_stationPortrait2, g_dw_stationPortrait3, g_dw_stationPortrait4,
    g_dw_stationPortrait5, g_dw_stationPortrait6, g_dw_stationPortrait7, g_dw_stationPortrait8, g_dw_stationPortrait9,
    g_dw_stationPortrait10, g_dw_stationPortrait11, g_dw_stationPortrait12, g_dw_stationPortrait13, g_dw_stationPortrait14,
    g_dw_stationPortrait15, g_dw_stationPortrait16, g_dw_stationPortrait17, g_dw_stationPortrait18, g_dw_stationPortrait19,
    g_dw_stationPortrait20, g_dw_stationPortrait21, g_dw_stationPortrait22, g_dw_stationPortrait23, g_dw_stationPortrait24,
    g_dw_stationPortrait25, g_dw_stationPortrait26, g_dw_stationPortrait27, g_dw_stationPortrait28, g_dw_stationPortrait29,
    g_dw_stationPortrait30, g_dw_stationPortrait31, g_dw_stationPortrait32, g_dw_stationPortrait33, g_dw_stationPortrait34,
    g_dw_stationPortrait35, g_dw_stationPortrait36, g_dw_stationPortrait37, g_dw_stationPortrait38, g_dw_stationPortrait39,
    g_dw_stationPortrait40, g_dw_stationPortrait41, g_dw_stationPortrait42, g_dw_stationPortrait43, g_dw_stationPortrait44,
    g_dw_stationPortrait45, g_dw_stationPortrait46, g_dw_stationPortrait47, g_dw_stationPortrait48, g_dw_stationPortrait49,
    g_dw_stationPortrait50, g_dw_stationPortrait51, g_dw_stationPortrait52, g_dw_stationPortrait53, g_dw_stationPortrait54,
    g_dw_stationPortrait55, g_dw_stationPortrait56, g_dw_stationPortrait57, g_dw_stationPortrait58, g_dw_stationPortrait59,
    g_dw_stationPortrait60, g_dw_stationPortrait61, g_dw_stationPortrait62
};

void DialogueWindow::loadContent() {
    GameText *gameText = Globals::gameText;
    FModSound *sound = Globals::sound;

    this->nextButton->replaceTextKeepSize(*gameText->getText(0xb4));
    this->mirrorFace = 0;
    this->autoAdvanceTimer = 0;
    this->pauseLength = 0;
    sound->stop(this->voiceSound);
    this->voiceSound = -1;
    this->nextButton->setPressProgress(0);

    if (this->faceParts != 0) {
        ArrayReleaseClasses(*this->faceParts); ArrayRemoveAll(*(this->faceParts));
        delete this->faceParts;
        this->faceParts = 0;
    }

    Mission *mission = this->mission;
    int kind = this->kind;
    int page = this->page;
    int textId = -1;

    if (mission != 0 && mission->isCampaignMission() != 0) {
        int base;
        if (kind == 0) {
            base = this->briefingOffsets[this->campaignMission];
            int row = base + page * 2;
            int portraitId = g_dw_campaignBriefingRows[row];
            textId = g_dw_campaignBriefingRows[row + 1];
            this->clientImage = g_dw_stationPortraits[portraitId];
            this->agentName = *gameText->getText(0x63d + portraitId);
            this->mirrorFace = portraitId == 0;
            this->bodyText = *gameText->getText(textId);
            unsigned textIndex = static_cast<unsigned>(row + 1);
            // ARM 0x16759c: only these briefing entries pass through the binding helper.
            if ((textIndex <= 23 && (0xA0200Au & (1u << textIndex)) != 0) || textIndex == 35) {
                this->bodyText = static_cast<Globals *>(Globals::globals)->replaceKeyBindingTokens(this->bodyText);
            }
        } else if (kind == 1) {
            base = this->successOffsets[this->campaignMission];
            if (this->campaignMission >= 1 && this->campaignMission <= 123) {
                int row = base + page * 2;
                int portraitId = g_dw_campaignSuccessRows1To123[row];
                textId = g_dw_campaignSuccessRows1To123[row + 1];
                this->clientImage = g_dw_stationPortraits[portraitId];
                this->agentName = *gameText->getText(0x63d + portraitId);
                this->mirrorFace = portraitId == 0;
            } else {
                textId = g_dw_campaignSuccessTextIds[base + page * 2 + 1];
                this->clientImage = g_dw_defaultClientImage;
                this->agentName = *gameText->getText(0x63d + (textId & 0xff));
            }
            this->bodyText = *gameText->getText(textId);
            if (textId == 1718 || textId == 1730) {
                this->bodyText = static_cast<Globals *>(Globals::globals)->replaceKeyBindingTokens(this->bodyText);
            }
            if (textId == 1833) {
                sound->stop(sound->currentMusicEvent);
                sound->play(0x88, 0, 0, 0.0f);
                sound->play(0xA2, 0, 0, 0.0f);
            }
            if (textId == 2782 && GameText::getLanguage() == 1) {
                this->clientImage = g_dw_defaultClientImage;
                this->agentName = *gameText->getText(0x63d);
            }
            if ((textId == 2193 || textId == 2194) && mission->getStatusValue() == 1) {
                textId -= 2;
                this->bodyText = *gameText->getText(textId);
            }
        } else {
            this->clientImage = g_dw_stationPortraits[16];
            this->agentName = *gameText->getText(0x63d + 16);
            String extra("");
            if (this->campaignMission >= 38 && this->campaignMission <= 41 &&
                this->campaignMission != 39) {
                extra = *gameText->getText(527);
            }
            this->bodyText = *gameText->getText(392) + String("\n", false) +
                             extra + String("\n\n", false) + *gameText->getText(319);
            textId = -1;
        }
    } else if (mission != 0) {
        if ((page & 1) != 0) {
            this->clientImage = g_dw_defaultClientImage;
            this->agentName = *(gameText->getText(0x63d));
            this->mirrorFace = 1;
        } else {
            this->clientImage = (void *) (intptr_t) mission->getClientImage();
            this->agentName = mission->getClientName();
            this->mirrorFace = 0;
        }

        bool fixedIntroText = false;
        if (kind == 0) {
            if (mission->getType() == 12) {
                textId = 372;
                fixedIntroText = true;
            } else if (mission->getType() == 7) {
                textId = 378;
                fixedIntroText = true;
            }
        }
        bool specialStation = false;
        if (!fixedIntroText && kind == 0 && mission->getTargetStation() == 0x6c) {
            const int *rows = Globals::status->field_114 == 2
                                  ? g_dw_specialStationStatus2 : g_dw_specialStationDefault;
            int portraitId = rows[page * 2];
            textId = rows[page * 2 + 1];
            this->bodyText = *gameText->getText(textId);
            this->clientImage = g_dw_stationPortraits[portraitId];
            this->agentName = *gameText->getText(0x63d + portraitId);
            specialStation = true;
        }
        bool failureQuantityText = false;
        if (kind != 0 && kind != 1 && mission->getType() == 12) {
            textId = 371;
            String first = Globals::status->replaceHash(
                String(*gameText->getText(textId), false),
                String(this->level->killCountB), String("#Q1", false));
            this->bodyText = Globals::status->replaceHash(
                first, String(this->level->killCountA), String("#Q2", false));
            failureQuantityText = true;
        }
        bool fixedSuccessText = false;
        if (kind == 1) {
            int missionType = mission->getType();
            if (missionType == 5 || missionType == 3) {
                textId = 389;
                this->bodyText = Globals::status->replaceHash(
                    String(*gameText->getText(textId), false),
                    mission->getTargetStationName(), String("#S", false));
                fixedSuccessText = true;
            } else if (missionType == 12) {
                textId = 370;
                String suffix("");
                Agent *agent = mission->getAgent();
                if (agent != 0 && agent->isStoryAgent()) {
                    suffix = String("\n\n", false) + Globals::status->replaceHash(
                        String(*gameText->getText(389), false),
                        agent->getStationName(), String("#S", false));
                }
                String body = String(*gameText->getText(textId), false) + suffix;
                String first = Globals::status->replaceHash(
                    body, String(this->level->killCountB), String("#Q1", false));
                this->bodyText = Globals::status->replaceHash(
                    first, String(this->level->killCountA), String("#Q2", false));
                fixedSuccessText = true;
            } else if (missionType == 0xb7) {
                int variant = static_cast<AERandom *>(Globals::rnd)->nextInt(5);
                textId = g_dw_genericTextIds[5 + variant];
                this->bodyText = *gameText->getText(textId);
                String suffix = String("\n\n", false) + *gameText->getText(176) +
                                String(": ", false) + String(this->level->killCountB);
                this->bodyText = this->bodyText + suffix;
                fixedSuccessText = true;
            }
        }
        if (!fixedIntroText && !specialStation && !failureQuantityText && !fixedSuccessText &&
            (kind == 1 || kind == 0 || kind == 2)) {
            Agent *agent = mission->getAgent();
            if (GameText::getLanguage() == 1 && agent != 0) {
                textId = this->pickGermanGenericTextBecauseWeSaved100EurosWithThat(kind, agent);
            } else {
                int variant = static_cast<AERandom *>(Globals::rnd)->nextInt(5);
                if (kind == 0) textId = g_dw_genericTextIds[variant];
                else if (kind == 1) textId = g_dw_genericTextIds[5 + variant];
                else textId = g_dw_genericTextIds[10 + variant];
            }
        } else if (!fixedIntroText && !specialStation && !failureQuantityText && !fixedSuccessText) {
            textId = 0x20f;
        }
        if (!specialStation && !failureQuantityText && !fixedSuccessText)
            this->bodyText = *gameText->getText(textId);

        if (kind == 1) {
            if (!fixedSuccessText) {
                String suffix("\n\n", false);
                Agent *agent = mission->getAgent();
                if (agent != 0 && agent->isStoryAgent()) {
                    suffix = suffix + Globals::status->replaceHash(
                        String(*gameText->getText(389), false),
                        agent->getStationName(), String("#S", false));
                } else {
                    suffix = suffix + *gameText->getText(216);
                }
                this->bodyText = this->bodyText + suffix;
            }
            Standing *standing = Status::gStatus->getStanding();
            standing->applyMissionCompleted(mission->getClientRace());
            if (mission->getTargetStation() == 0x6c) {
                textId = 458;
                this->bodyText = *gameText->getText(textId);
            }
        }
    } else {
        this->clientImage = g_dw_defaultClientImage;
        textId = 0x10;
        this->bodyText = *(gameText->getText(textId));
        this->agentName = *(gameText->getText(0x63d));
    }

    String style(g_dw_emptyLoad, false);
    String body;
    body.Set((this->bodyText).data);
    this->scrollWindow->setText(style, body, 0);

    this->prevButton->setVisible(this->page != 0);
    this->moreButton->setVisible(this->length() > 1);
    this->faceParts = ((ImageFactory *) Globals::imageFactory)->loadChar((int *) this->clientImage);

    if (this->isLastPage() != 0) {
        this->nextButton->replaceTextKeepSize(*gameText->getText(0xb5));
    }

    Agent *agent = mission == 0 ? (Agent *) 0 : mission->getAgent();
    int soundId = Globals::gGlobals->getDialogueSoundId(textId, agent);
    this->voiceSound = soundId;
    if (soundId >= 0) {
        sound->play(soundId, 0, 0, 0);
        this->pauseLength = sound->getEventPauseLength(soundId);
    }
}

DialogueWindow::DialogueWindow() {
    { if (this->bodyText.data) delete[] this->bodyText.data; this->bodyText.data = nullptr; this->bodyText.length = 0; }
    { if (this->agentName.data) delete[] this->agentName.data; this->agentName.data = nullptr; this->agentName.length = 0; }
    this->init();
}


void DialogueWindow::update(int dt) {
    if (this->scrollWindow != 0) {
        this->scrollWindow->update(dt);
    }
    if (this->choiceActive != 0) {
        this->choiceWindow->update(dt);
    }
    if (Globals::options[0x0e] != 0 && this->voiceSound != -1) {
        Globals::sound->getPlayingProgress(this->voiceSound);
        if (Globals::sound->isPlaying(this->voiceSound) == 0 && this->isLastPage() == 0) {
            if (this->autoAdvanceTimer >= this->pauseLength) {
                this->nextPage();
            }
            this->autoAdvanceTimer = this->autoAdvanceTimer + dt;
        }
    }
}


int DialogueWindow::OnTouchEnd(int x, int y) {
    if (this->choiceActive != 0) {
        int r = this->choiceWindow->OnTouchEnd(x, y);
        if (r != 1) {
            if (r != 0) return 0;
            this->choiceActive = 0;
            if (Status::gStatus->getCurrentCampaignMission() == 0x0f) {
                Globals::sound->play(0xa2, 0, 0, 0);
                Globals::sound->stop(Globals::sound->currentMusicEvent);
                Globals::sound->play(0x88, 0, 0, 0);
            }
            if (this->voiceSound != -1) {
                Globals::sound->stop(this->voiceSound);
            }
            return 1;
        }
        this->choiceActive = 0;
        return 0;
    }

    this->scrollWindow->OnTouchEnd(x, y);
    if (this->prevButton->OnTouchEnd(x, y) != 0) {
        Globals::sound->stop(this->voiceSound);
        this->previousPage();
    }
    if (this->nextButton->OnTouchEnd(x, y) != 0) {
        Globals::sound->stop(this->voiceSound);
        if (this->nextPage() == 0) {
            return 1;
        }
    }
    if (this->moreButton->OnTouchEnd(x, y) != 0) {
        ChoiceWindow *choiceWindow = this->choiceWindow;
        String *text = Globals::gameText->getText(0x18c);
        choiceWindow->set(*text, true);
        this->choiceActive = 1;
    }
    return 0;
}


static const char g_dw_defaultAgentName[] = "";

static inline int half_round_to_zero(int v) {
    return (v + ((unsigned) v >> 31)) >> 1;
}

int DialogueWindow::init() {
    this->briefingOffsets = new int[0xa2];
    this->successOffsets = new int[0xa2];

    int briefingSum = 0;
    int successSum = 0;
    for (int i = 0; i != 0xa2; ++i) {
        int briefingCount = g_dw_briefingCounts[i];
        int successCount = g_dw_successCounts[i];
        this->briefingOffsets[i] = briefingSum;
        this->successOffsets[i] = successSum;
        briefingSum += briefingCount;
        successSum += successCount;
    }

    this->agentName = String(g_dw_defaultAgentName, false);

    this->mission = 0;
    this->choiceWindow = 0;
    this->level = 0;
    this->clientImage = 0;
    this->faceParts = 0;
    this->voiceSound = -1;
    this->autoAdvanceTimer = 0;
    this->mirrorFace = 0;

    Layout *layout = Globals::layout;
    int frameW = layout->field_0x54;
    int frameH = layout->field_0x58;
    this->frameWidth = frameW;
    this->frameHeight = frameH;
    this->frameX = half_round_to_zero(Globals::w) - half_round_to_zero(frameW);
    this->frameY = half_round_to_zero(Globals::h) - half_round_to_zero(frameH);

    int margin = layout->field_0x4c;
    this->scrollWindow = new ScrollTouchWindow(
        this->frameX + margin * 2 + layout->field_0x2d4,
        layout->field_0x8 + this->frameY,
        frameW - margin * 2 - layout->field_0x2d4,
        frameH - margin * 2 - layout->field_0x8 - layout->field_0x30,
        false);

    this->choiceWindow = new ChoiceWindow();

    GameText *gameText = Globals::gameText;
    layout = Globals::layout;
    margin = layout->field_0x4c;
    this->prevButton = new TouchButton(*gameText->getText(0xb3), 5,
                                       this->frameX + margin, this->frameY - margin + this->frameHeight,
                                       layout->field_0x50, 0x21, 4);

    layout = Globals::layout;
    margin = layout->field_0x4c;
    this->nextButton = new TouchButton(*gameText->getText(0xb4), 6,
                                       this->frameX + this->frameWidth - margin,
                                       this->frameY - margin + this->frameHeight,
                                       layout->field_0x50, 0x22, 4);

    layout = Globals::layout;
    margin = layout->field_0x4c;
    this->moreButton = new TouchButton(*gameText->getText(0x18b), 0,
                                       this->frameX + half_round_to_zero(this->frameWidth),
                                       this->frameY + this->frameHeight - margin,
                                       this->frameWidth - margin * 4 - layout->field_0x50 * 2, 0x24, 4);
    this->choiceActive = 0;
    return 0;
}


static const char g_dw_emptyString[] = "";

DialogueWindow::DialogueWindow(String *text, String *agentName, int *parts) {
    { if (this->bodyText.data) delete[] this->bodyText.data; this->bodyText.data = nullptr; this->bodyText.length = 0; }
    { if (this->agentName.data) delete[] this->agentName.data; this->agentName.data = nullptr; this->agentName.length = 0; }
    this->init();

    String blank(g_dw_emptyString, false);
    String copy;
    copy.Set((text)->data);
    this->scrollWindow->setText(blank, copy);

    this->moreButton->setVisible(false);
    this->prevButton->setVisible(false);

    this->faceParts = ((ImageFactory *) Globals::imageFactory)->loadChar(parts);
    delete this->nextButton;
    this->nextButton = 0;

    GameText *gameText = Globals::gameText;
    Layout *layout = Globals::layout;
    int margin = layout->field_0x4c;
    int x = this->frameX + this->frameWidth / 2;
    int y = this->frameY + this->frameHeight - margin;
    int width = this->frameWidth - margin * 4 - layout->field_0x50 * 2;
    this->nextButton = new TouchButton(*gameText->getText(0x20c), 0, x, y, width, 0x24, 4);

    this->agentName = *(agentName);
    this->voiceSound = -1;
    this->page = 0;
    this->pauseLength = 0;
}

DialogueWindow::DialogueWindow(Mission *mission, Level *level, int kind) {
    { if (this->bodyText.data) delete[] this->bodyText.data; this->bodyText.data = nullptr; this->bodyText.length = 0; }
    { if (this->agentName.data) delete[] this->agentName.data; this->agentName.data = nullptr; this->agentName.length = 0; }
    this->init();
    this->level = level;
    this->set(mission, kind, -1);
}


static const int g_dw_germanTexts[62] = {
    379, 380, 379, 380, 380, 383, 380, 383, 379, 380,
    379, 380, 379, 381, 380, 382, 379, 380, 379, 380,
    387, 388, 384, 385, 384, 385, 384, 385, 385, 387,
    384, 385, 384, 386, 385, 386, 384, 385, 384, 385,
    373, 376, 373, 374, 374, 376, 374, 376, 373, 374,
    373, 374, 373, 375, 373, 375, 373, 374, 373, 374,
    2376, 2377
};

int DialogueWindow::pickGermanGenericTextBecauseWeSaved100EurosWithThat(int kind, Agent *agent) {
    int race = agent->getRace();
    int male;
    if (race < 10) {
        race = agent->getRace();
        male = agent->isMale();
        if (race == 3) {
            if (agent->getImageParts() == 0) {
                race = 3;
            } else {
                int *parts = agent->getImageParts();
                race = 0;
                if (*parts == 2) race = 3;
            }
        }
    } else {
        male = agent->isMale();
        race = 10;
    }

    int index;
    const int *texts;
    if (kind == 2) {
        index = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
        texts = g_dw_germanTexts + 20;
    } else if (kind != 0) {
        index = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
        texts = g_dw_germanTexts + 40;
    } else {
        index = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
        texts = g_dw_germanTexts;
    }

    const int *picked = &texts[18 + index];
    if (male != 0) {
        picked = &texts[race * 2 + index];
    }
    return *picked;
}


void DialogueWindow::draw() {
    Globals::Canvas->SetColor((unsigned int) -1);
    Layout *layout = Globals::layout;
    layout->drawMask();
    String title(this->agentName, false);
    layout->drawBox(7, this->frameX, this->frameY, this->frameWidth, this->frameHeight, title);

    this->scrollWindow->draw();

    layout = Globals::layout;
    int margin = layout->field_0x4c;
    ((ImageFactory *) Globals::imageFactory)->drawChar(this->faceParts,
                                                       this->frameX + margin, this->frameY + margin + layout->field_0x8,
                                                       this->mirrorFace);

    this->prevButton->draw();
    this->nextButton->draw();
    this->moreButton->draw();

    if (this->choiceActive != 0) {
        this->choiceWindow->draw();
    }

    if (Globals::is_choice_window_visible == 0) {
        Vector pos;
        if (this->moreButton != 0) {
            pos = this->moreButton->getPosition();
            Globals::other_buttons_x[2] = (int) pos.x;
            pos = this->moreButton->getPosition();
            Globals::other_buttons_y[2] = (int) pos.y;
        }
        if (this->nextButton != 0) {
            pos = this->nextButton->getPosition();
            Globals::other_buttons_x[3] = (int) pos.x;
            pos = this->nextButton->getPosition();
            Globals::other_buttons_y[3] = (int) pos.y;
        }
        Globals::is_dialogue_window_visible = 1;
    }
}

void DialogueWindow::setLevel(Level *level) {
    this->level = level;
}
