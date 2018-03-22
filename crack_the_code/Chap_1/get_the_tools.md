---
layout: default
title: Get the tools
---
## [](#header-2) Get the tools

Currently Prelude only compiles on Windows. It uses a subset of Chromium source, which requires the same tools to build:

*  **depot_tools**
*  **Visual Studio 2017**
*  **Windows 10 SDK**

This post outlines the steps to setup these tools. It is assumed that the system is running Windows 7 or above, preferably with latest update.

### [](#header-3) 1. Download [**depot_tools**](https://storage.googleapis.com/chrome-infra/depot_tools.zip) and unzip it
For simplicity, assume it's unzipped to *c:\depot_tools*
### [](#header-3) 2.  Add the *depot_tools* directory to the *PATH* environment variable
*  Open *Control Panel -> System and Security -> System*

![Open control panel]({{ site.baseurl }}/crack_the_code/images/adv_sys_settings.png){:class="img-responsive"}

*  In *System Properties* dialog, click *Environment Variables...*

![Open env dialog]({{ site.baseurl }}/crack_the_code/images/env.png){:class="img-responsive"}

* Add the full path of *depot_tools* to *PATH* environment variable:

![Add to PATH ]({{ site.baseurl }}/crack_the_code/images/add_depot_tools_to_path.png){:class="img-responsive"}

### [](#header-3) 3.  Setup environment variable
On *Environment Variables* dialog window, click *New...* to create a new environment variable - *DEPOT_TOOLS_WIN_TOOLCHAIN*, and set its value to 0

![Create Env var]({{ site.baseurl }}/crack_the_code/images/new_env.png){:class="img-responsive"}

![Set env var value]({{ site.baseurl }}/crack_the_code/images/add_env.png){:class="img-responsive"}

### [](#header-3) 4.  Setup *gclient*
Open a console, go to *depot_tools* directory and run *gclient*:

![Run Gclient]({{ site.baseurl }}/crack_the_code/images/run_gclient.png){:class="img-responsive"}

*depot_tools* provides essential tools such as *python (v2.7.6)* and *git (v2.15.1.windows.2)*. Running *gclient* without any arguments downloads these tools. Once you add *depot_tools* directory to *PATH*, these tools are ready to use. You may open a console and run: *where python* to confirm that Windows knows where to find Python executable.

![Check after gclient]({{ site.baseurl }}/crack_the_code/images/check_after_gclient.png){:class="img-responsive"}

When *python* or *git* is invoked, the corresponding batch file (shown above) forward the call to the actual executables under *depot_tools\win_toos-2_7_6_bin*. Chromium requires that these batch files take precedence over local executables that are previously installed. As seen in figure below, *Python v2.7.14* has been installed and *where* command returns the batch file before v2.7.14 executable. This can be done by moving *c:\depot_tools* before *c:\Python27* in *PATH* environment variable. Prelude does not have this restriction, i.e., using *python v2.7.14* instead of the bundled one in depot_tools should still work.

![depot_tool_batch_files]({{ site.baseurl }}/crack_the_code/images/python_local.png){:class="img-responsive"}

### [](#header-3) 5.  Install Visual Studio 2017
Community version works fine. Make sure following components are installed:
* VC++ 2017 v141 toolset
* Windows 10 SDK (10.0.16299)
* Visual C++ ATL support
* MFC and ATL support

![Install VS2017]({{ site.baseurl }}/crack_the_code/images/vs2017_install.png){:class="img-responsive"}

As of Febuary 2018, Visual Studio 2017 available from the official site is v15.5.6. For Prelude, Visual Studio v15.5.x with SDK v10.0.16299 alone is sufficient. To build the whole Chromium, install SDK v10.0.15063.0 as well. Different versions of Visual Studio and Windows SDK can cause some issues, which will be elaborated in a later post.
### [](#header-3) 6.  Add **Debugging Tools** for Windows SDK

Open *Control Panel* and go to *Programs and Festures*, selet *Windows Software Development Kit* then click *Change*. It will run the SDK setup again that allow you to add debugging tools.

![add_debug_tools_1]({{ site.baseurl }}/crack_the_code/images/control_panel_change_sdk.png){:class="img-responsive"}

![add_debug_tools_2]({{ site.baseurl }}/crack_the_code/images/sdk_install_debug_tools.png){:class="img-responsive"}

Now we have setup all the tools, next we will build Prelude from scratch.

[Back]({{ site.baseurl }}/)
