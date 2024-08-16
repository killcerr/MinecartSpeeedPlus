#include <Nlohmann/json.hpp>
#include <filesystem>
#include <ll/api/Logger.h>

ll::Logger logger("MinecartSpeedPlus");

struct {
    float GoldenRailMul;
    float CommonRailMul;
} gcfg;

void ModInit() {
    std::filesystem::create_directories("./plugins/MinecartSpeedPlus/");
    if (std::filesystem::exists("./plugins/MinecartSpeedPlus/cfg.json")) {
        std::ifstream fin("./plugins/MinecartSpeedPlus/cfg.json");
        std::string   c, l;
        while (std::getline(fin, l)) {
            c += l + '\n';
        }
        auto cfg           = nlohmann::json::parse(c, nullptr, false, true);
        gcfg.GoldenRailMul = cfg["GoldenRailMul"].get<float>();
        gcfg.CommonRailMul = cfg["CommonRailMul"].get<float>();
    } else {
        std::ofstream fout("./plugins/MinecartSpeedPlus/cfg.json");
        fout << R"({"GoldenRailMul":0.0,"CommonRailMul":0.0})";
        gcfg.GoldenRailMul = 0.0;
        gcfg.CommonRailMul = 0.0;
    }
    logger.info("MinecartSpeeedPlus by killcerr loaded");
}

#include <ll/api/memory/Hook.h>
#include <mc/entity/utilities/RailMovementUtility.h>
#include <mc/math/Vec3.h>


bool flag = false;

LL_AUTO_TYPE_STATIC_HOOK(
    calculateGoldenRailSpeedIncreaseHook,
    HookPriority::Normal,
    RailMovementUtility,
    "?calculateGoldenRailSpeedIncrease@RailMovementUtility@@SA?AVVec3@@AEBVIConstBlockSource@@AEBVBlockPos@@HV2@@Z",
    Vec3,
    class IConstBlockSource const& a,
    class BlockPos const&          b,
    int                            c,
    class Vec3                     d
) {
    auto res = origin(a, b, c, d);
    flag     = true;
    return res;
}

LL_AUTO_TYPE_STATIC_HOOK(
    calculateMoveVelocityHook,
    HookPriority::Normal,
    RailMovementUtility,
    "?calculateMoveVelocity@RailMovementUtility@@SA?AVVec3@@AEBVBlock@@HM_NAEAV2@AEA_N3AEBV?$function@$$A6A_NAEAVVec3@@"
    "@Z@std@@@Z",
    Vec3,
    class Block const&                            a,
    int                                           b,
    float                                         c,
    bool                                          d,
    class Vec3&                                   e,
    bool&                                         f,
    bool&                                         g,
    class std::function<bool(class Vec3&)> const& h
) {
    auto res = origin(a, b, c, d, e, f, g, h);
    if (!flag) res *= gcfg.CommonRailMul;
    else {
        flag  = false;
        res  *= gcfg.GoldenRailMul;
    }
    return res;
}
