#ifndef __MQTT_COMPONENT_H__
#define __MQTT_COMPONENT_H__

#include <ArduinoJson.h>

namespace Rovi {
    namespace Components {
        template<class C>
        class MqttComponent {
            public:
                virtual void status(char* json, size_t maxSize) const = 0;

                static size_t jsonSize() { return getMaxSize<C>(0); }

                static const size_t MAX_SIZE = 0;       // Must be public due to template, inheritance and access rights... :(
            protected:
                template<typename T>
                static size_t getMaxSize(decltype(T::MAX_SIZE)*) { return T::MAX_SIZE; }

                template<typename T>
                static void getMaxSize(...) { return MAX_SIZE; }

        };
    };
};

#endif /* __MQTT_COMPONENT_H__ */