---
layout: default
title: Service 101
---
## [](#header-2) Service 101
As shown in last section, Mojo interface pointer need to be bound to the implementation before it can be used. However, this should not be the clients' job. The only thing clients need is the Mojo interface (i.e., the generated headers), so that it can happily make calls through Mojo without concerning itself with implementation details.

Service is the middle man - or broker - that fills in the gap. It provides following functionalities:
* Bind Mojo interface to its implementation
* Manage/broker Mojo requests during runtime
* Lifecycle management for spawned processes
> Service is currently under active development. The code provided here is based on codebase 71.0.3559.3, some of which is deprecated. However, the concept is still valid. It greatly eases the learning curve when study the up-to-date codebase.

### [](#header-3) Intra-process service
First, let's implement *FortuneCookie* interface with a new class:
```c++
class VanillaFortuneCookie : public mojom::FortuneCookie {
 public:
  VanillaFortuneCookie();
  ~VanillaFortuneCookie() override;

  static void Create(mojom::FortuneCookieRequest request);

 private:
  void Crack(CrackCallback callback) override;

  DISALLOW_COPY_AND_ASSIGN(VanillaFortuneCookie);
};
```
Note that there's no binding object declared in the header file, the secret is in its *Create()* function which replaces *BindRequest()*:

```c++
void VanillaFortuneCookie::Create(mojom::FortuneCookieRequest request) {
  mojo::MakeStrongBinding(std::make_unique<VanillaFortuneCookie>(),
                          std::move(request));
}
```
This is another pattern used a lot in Chromium. *mojo::MakeStrongBinding()* creates a *strong binding object* that owns the implementation instance and clean itself up if error is detected, or it's no longer needed.

Binding interface to the implementation class is done by *service_manager::Service* or its sub-class:
```c++
class VanillaCookieService : public service_manager::Service {
 public:
  VanillaCookieService();
  ~VanillaCookieService() override;

 private:
  // service_manager::Service
  void OnStart() override;
  void OnBindInterface(const service_manager::BindSourceInfo& source_info,
                       const std::string& interface_name,
                       mojo::ScopedMessagePipeHandle interface_pipe) override;

  void BindCookieService(mojom::FortuneCookieRequest request);

  service_manager::BinderRegistry registry_;

  DISALLOW_COPY_AND_ASSIGN(VanillaCookieService);
};
```

`VanillaCookieService` has a member variable `service_manager::BinderRegistry registry_` which passes the binding information to the service manager.

```c++
void VanillaCookieService::OnStart() {
  registry_.AddInterface<mojom::FortuneCookie>(base::Bind(
      &VanillaCookieService::BindCookieService, base::Unretained(this)));
}

void VanillaCookieService::BindCookieService(
    mojom::FortuneCookieRequest request) {
  VanillaFortuneCookie::Create(std::move(request));
}
```
In *OnStart()*. `VanillaCookieService` tells the manager that when first request for `FortuneCookie` interface arrives, run *VanillaFortuneCookie::Create()* to create and bind an implementation instance.

In BUILD.gn, these two classes are compiled together as a source set:
```gn
source_set("lib") {
  sources = [
    "vanilla_fortune_cookie.h",
    "vanilla_fortune_cookie.cc",
    "vanilla_cookie_service.h",
    "vanilla_cookie_service.cc",
  ]

  deps = [
    "//base",
    "//prelude/excerpt02_mojo/public/mojom"
  ]
}
```

Before making use of the newly created service, there are two things to note:
1. Only a service can talk to another service. That means a client itself must have a service embedded somewhere too. For intra-process application, the client can simply *embed* the service it wants to use.

2. Each service must define a manifest, which tells the service manager what it provides and/or what it needs.

![chrome process]({{ site.baseurl }}/crack_the_code/Chap_3/images/manifest.png)

The image above shows the manifest for this example. There are three types of names surrounded in boxes with different colors:

|Color | Type| Name|
|:----|:----|:-----|
|Red|Service|cookie_dine_in_service|
|Green|Capability|vanilla_cookie_dine_in|
|Blue|Mojo interface|prelude.mojom.FortuneCookie|

Notice that because the client embeds `VanillaCookieService`, it *provides* a capability named *vanilla_cookie_dine_in* which implements *FortuneCookie* interface. On the other hand, it requies this capability in order to access *FortuneCookie*.

Also, note that the Mojo interface name only appears in *provides* section, while *requires* specifies service name and capability name.

Below is the build target for manifest in BUILD.gn:
```gn
service_manifest("dine_in_manifest") {
  name = "cookie_dine_in_service"
  source = "cookie_dine_in_manifest.json"
}
```
The manifest *name* **must** match the *service name* specified in the manifest json file (i.e., *cookie_dine_in_service*). With manifest file in place, it's time to build the catalog. Catalog is like a library that maintain a collection of manifests related to the client. It does not have any source files, just a build target which pointing to the manifest tagets (target name defined in BUILD.gn, **not** the *service name*) it depends on as *embedded_services*:
```gn
catalog("dine_in_catalog") {
  embedded_services = [":dine_in_manifest"]
}

copy("copy_dine_in_catalog") {
  sources = get_target_outputs(":dine_in_catalog")
  outputs = ["$root_out_dir/dine_in_catalog.json"]
  deps = [":dine_in_catalog"]
}
```
Build target *dine_in_catalog* generates a *dine_in_catalog.json* which looks much like the input manifest file:
```json
{
  "services": {
    "cookie_dine_in_service": {
      "embedded": true,
      "manifest": {
        "display_name": "Dine in",
        "name": "cookie_dine_in_service",
        "interface_provider_specs": {
          "service_manager:connector": {
            "requires": {
              "cookie_dine_in_service": [
                "vanilla_cookie_dine_in"
              ]
            },
            "provides": {
              "vanilla_cookie_dine_in": [
                "prelude.mojom.FortuneCookie"
              ]
            }
          }
        }
      }
    }
  }
}
```
Information in this json file will be loaded to the client in runtime so that service manager knows how to handle the service request for *prelude.mojom.FortuneCookie*. The copy target *copy_dine_in_catalog* copies this file from generated file directory to the output folder.

Finally, it's time to take a look at the client code. In *main()* before the line `mojo::core::Init();`, the generated catalog json file is loaded into a `base::Value` to be used later. The next three calls are the key, which involves a couple of classes not seen before:
```c++
  auto background_service_manager(
      std::make_unique<service_manager::BackgroundServiceManager>(
          nullptr, std::move(json_value)));

  service_manager::mojom::ServicePtr service;
  auto service_ctx(std::make_unique<service_manager::ServiceContext>(
      std::make_unique<prelude::VanillaCookieService>(),
      mojo::MakeRequest(&service)));

  background_service_manager->RegisterService(
      service_manager::Identity(kServiceName,
                                service_manager::mojom::kRootUserID),
      std::move(service), nullptr);
```

Compare with the client code in last section, a service instance (*VanillaCookieService*) is created instead of the implemetation class, and immediately passed to the constructor of *ServiceContext*. Consequently, binding interface (*prelude.mojom.FortuneCookie*) is done with this new class instead of through a call to the implementation class:
```c++
  prelude::mojom::FortuneCookiePtr cookie;
  service_ctx->connector()->BindInterface(kServiceName, &cookie);
```
*BackgroundServiceManager* is the manager that takes in the catalog data in its constructor and manages services in a background service. When register a service, an instance of type *Identity* has to be provided along with the service instance. Make sure that the first parameter passed in the constructor is the service name specified in manifest file. The same apply to `BindInterface()` call, both takes the service name `cookie_dine_in_service`.

The rest is similar to the previous samples. The message from the cookie is printed out:
```
cookie says: Your road to glory will be rocky, but fulfilling.
```

### [](#header-3) Inter-process service
Calling services from an other process requires that service be packaged into an executable on its own. Let's start with converting `VanillaCookieService` into a stand-alone service. It involves providing one function that will be hooked up in *main()*.
```c++
MojoResult ServiceMain(MojoHandle service_request_handle) {
  return service_manager::ServiceRunner(new prelude::VanillaCookieService)
      .Run(service_request_handle);
}
```
Since `VanillaCookieService` is a service on its own right, it requires a manifest for itself:
```json
{
  "name": "cookie_delivery_service",
  "display_name" : "Cookie delivery",
  "interface_provider_specs" : {
    "service_manager:connector" : {
      "provides": {
        "vanilla_cookie_delivery": [
          "prelude.mojom.FortuneCookie"
        ]
      }
    }
  }
}
```
Note that it provides a `vanilla_cookie_delivery` capability without requiring anything.
The service executable is built with target type *service* as seen below. On windows, the generated services always ends with *service.exe*. For example, a *cookie_delivery_service.service.exe* will be generated here.
```gn
service("cookie_delivery_service") {
  sources = [
    "cookie_delivery_service_main.cc"
  ]

  deps = [
    ":lib",
    "//services/service_manager/public/cpp",
  ]
}

service_manifest("cookie_delivery_manifest") {
  name = "cookie_delivery_service"
  source = "cookie_delivery_service_manifest.json"
}
```
For the client, as mentioned before, it needs to have its own service. And a service always comes with its manifest:
```json
{
  "name": "cookie_delivery_app",
  "display_name" : "Cookie delivery app",
  "interface_provider_specs" : {
    "service_manager:connector" : {
      "requires": {
        "cookie_delivery_service": ["vanilla_cookie_delivery"]
      }
    }
  }
}
```
The embedded service is named *cookie_delivery_app*, which requires *vanilla_cookie_delivery* capability from *cookie_delivery_service*. In BUILD.gn, *cookie_delivery_service* is no longer an *embedded* service, but a *stand-alone* type.

```gn
service_manifest("cookie_delivery_app_manifest") {
  name = "cookie_delivery_app"
  source = "cookie_delivery_app_manifest.json"
}

catalog("cookie_delivery_app_catalog") {
  embedded_services = [":cookie_delivery_app_manifest"]
  standalone_services = [":cookie_delivery_manifest"]
}
```
[*cookie_delivery_app.cc*](https://github.com/xzwang2005/Prelude/blob/master/prelude/excerpt02_mojo/service_101/cookie_delivery_app.cc) shows how to setup and use inter-process service.

A couple of things in client's code that are differnt from the intra-process sample:
1. Intead of explicit loading catalog json file into `base::Value` and pass it to BackgroundServiceManager, the catalog file is loaded with `catalog::Catalog::LoadDefaultCatalogManifest(catalog_path);`. This is just another way to get catalog data into the system.
2. More importantly, two more things need to be take care of, otherwise the app crashes in runtime: create an execution context and setup Mojo IPC support.
```c++
  // must have for inter-process communication
  base::TaskScheduler::CreateAndStartWithDefaultParams("cookie_delivery_app");
  mojo::core::Init();
  // must have for inter-process communication
  base::Thread ipc_thread_("IPC thread");
  ipc_thread_.StartWithOptions(
      base::Thread::Options(base::MessageLoop::TYPE_IO, 0));
  mojo::core::ScopedIPCSupport ipc_support_(
      ipc_thread_.task_runner(),
      mojo::core::ScopedIPCSupport::ShutdownPolicy::CLEAN);
```
We will talk about execution context in later posts.

3. The `ServiceContext` instance is created with a default `service_manager::Service` object, which is reponsible for requiesting services from other process during runtime.
```c++
  auto context = std::make_unique<service_manager::ServiceContext>(
      std::make_unique<service_manager::Service>(),
      mojo::MakeRequest(&service));
```
By now, the client knows nothing about the implementation. The only hint is that in BUILD.gn, the executable list the service as its *data_deps*:
```gn
  data_deps = [
    ":cookie_delivery_service"
  ]
```
*data_deps* means *cookie_delivery_service* is **not** linked to, but better be built before the current target. This is usually used for things that required during runtime.

The rest is the same as the intra-process service.
To try it out, run: `cookie_delivery_app.exe --wait-for-debugger`, the executable waits 60 seconds for a debugger to be attached. When the service process starts, it also pops up a window with the process id, waiting for a debugger:

![service process]({{ site.baseurl }}/crack_the_code/Chap_3/images/service_process.png)

While the service process is waiting, it might be interesting to check out all the processes in the system:

![cookie delivery process]({{ site.baseurl }}/crack_the_code/Chap_3/images/cookie_delivery_service.png)

As shown above, *cookie_delivery_app* was able to launch the service process for us, and get the same results as in last example.
