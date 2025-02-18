#ifndef _INCLUDE_SIGSEGV_MOD_ETC_MAPENTITY_ADDITIONS_H_
#define _INCLUDE_SIGSEGV_MOD_ETC_MAPENTITY_ADDITIONS_H_

#include "util/lua.h"
#include "util/prop_helper.h"

class PlayerUpgradeHistory;

namespace Mod::Etc::Mapentity_Additions
{

    enum GetInputType {
        ANY,
        VARIABLE,
        KEYVALUE,
        DATAMAP,
        SENDPROP,
        VARIABLE_NO_FIND,
        VARIABLE_NO_CREATE,
        DATAMAP_REFRESH,
    };

    bool SetEntityVariable(CBaseEntity *entity, GetInputType type, const char *name, variant_t &variable);
    bool SetEntityVariable(CBaseEntity *entity, GetInputType type, const std::string &name, variant_t &variable, int arrayPos, int vecAxis);
    
    bool GetEntityVariable(CBaseEntity *entity, GetInputType type, const char *name, variant_t &variable);
    bool GetEntityVariable(CBaseEntity *entity, GetInputType type, const std::string &name, variant_t &variable, int arrayPos, int vecAxis);

    extern bool allow_create_dropped_weapon;

    class DroppedWeaponModule : public EntityModule
    {
    public:
        DroppedWeaponModule(CBaseEntity *entity) : EntityModule(entity) {}

        CHandle<CBaseEntity> m_hWeaponSpawner;
        int ammo = -1;
        int clip = -1;
        float energy = FLT_MIN;
        float charge = FLT_MIN;
    };

    class FakeParentModule : public EntityModule
    {
    public:
        FakeParentModule(CBaseEntity *entity) : EntityModule(entity) {}
        CHandle<CBaseEntity> m_hParent;
        bool m_bParentSet = false;
        bool m_bDeleteWithParent = false;
    };

    class MathVectorModule : public EntityModule
    {
    public:
        MathVectorModule(CBaseEntity *entity) : EntityModule(entity) {}
        union {
            Vector value;
            QAngle valueAng;
        };
    };

    class RotatorModule : public EntityModule
    {
    public:
        CHandle<CBaseEntity> m_hRotateTarget;
    };

    class FakePropModule : public EntityModule, public AutoList<FakePropModule>
    {
    public:
        FakePropModule() {}
        FakePropModule(CBaseEntity *entity) : entity(entity) {}
        
        CBaseEntity *entity = nullptr;
        std::unordered_map<std::string, std::pair<variant_t, variant_t>> props;
    };
    
    class ForwardVelocityModule : public EntityModule
    {
    public:
        ForwardVelocityModule(CBaseEntity *entity);
    };

    class AimFollowModule : public EntityModule
    {
    public:
        AimFollowModule(CBaseEntity *entity) : EntityModule(entity) {}
        CHandle<CBaseEntity> m_hParent;
    };

    class ScriptModule : public EntityModule, public Util::Lua::LuaState
    {
    public:
        ScriptModule(CBaseEntity *entity) : EntityModule(entity), owner(entity) {}

        virtual void TimerAdded() override;
        CBaseEntity *owner;
        bool popInit = false;
    };

    class VisibilityModule : public EntityModule
    {
    public:
        VisibilityModule(CBaseEntity *entity) : EntityModule(entity) {}

        bool defaultHide = false;
        std::vector<edict_t *> hideTo;
    };


    using CustomInputFunction = void (*)(CBaseEntity *, const char *, CBaseEntity *, CBaseEntity *, variant_t &);
    CustomInputFunction *GetCustomInput(CBaseEntity *ent, const char *szInputName);

    class CustomInput {
    public:
        CustomInput(std::string_view name, bool prefix, CustomInputFunction func) : name(name), prefix(prefix), func(func) {}
        std::string_view name; 
        bool prefix;
        CustomInputFunction func;
    };

    class InputFilter : public AutoList<InputFilter> {
    public:
        InputFilter(std::vector<CustomInput> inputs) : 
        inputs(inputs) 
        {}
        
        virtual bool Test(CBaseEntity *ent) {return true;}
        
        void AddFunction(std::string_view name, bool prefix, CustomInputFunction func) {
            inputs.push_back({name, prefix, func});
        }
        std::vector<CustomInput> inputs;
    };

    class ClassnameFilter : public InputFilter {
    public:
        ClassnameFilter(const char *classname, std::vector<CustomInput> inputs) :InputFilter(inputs), classname(PooledString(classname)) {}

        virtual bool Test(CBaseEntity *ent) {return ent->GetClassname() == classname;}

        PooledString classname;
    };

    template<typename T>
    class RTTIFilter : public InputFilter {
    public:
        RTTIFilter(std::vector<CustomInput> inputs) : InputFilter(inputs) {}

        virtual bool Test(CBaseEntity *ent) {return rtti_cast<T>(ent);}
    };

    struct MyPlayerUpgradeHistory
    {
        CSteamID m_steamId;							// which player this snapshot is for
        std::vector< CUpgradeInfo > m_upgradeVector;	 
        int m_currencySpent;
    };
    struct ChangeLevelInfo
    {
        bool set = false;
        bool setInitialize = false;
        bool setInitializeCurrency = false;
        int startingCurrency = 0;
        std::string mission;
        int currencyCollected = 0;
        std::vector<MyPlayerUpgradeHistory> playerUpgradeHistory;
        std::map<CSteamID, std::set<int>> selectedLoadoutItems;
		std::map<CSteamID, std::set<int>> boughtLoadoutItems;
        int loadoutItemsCount;
    };
    extern ChangeLevelInfo change_level_info;

    void AddModuleByName(CBaseEntity *entity, const char *name);
    bool DoCollideTestInternal(CBaseEntity *entity1, CBaseEntity *entity2, bool &result, variant_t &val);

    extern int waveToJumpNextTick;
    extern float waveToJumpNextTickMoney;
}

#endif