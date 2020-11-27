.. _design_peripheral:

Peripheral
==========

.. _design_peripheral_overview:

Overview
--------

Regarding the peripheral support(such as UART, GPIO, SPI, I2C and etc.) in Nuclei SDK,
we didn't define a device or peripheral layer for different SoCs, so the peripheral drivers
are directly tighted with each SoC, and if developer want to use the drivers, they can directly
use the driver API defined in each SoC.

Considering this peripheral driver difference in each SoC, if you want to write portable code in
Nuclei SDK, you can use include the ``nuclei_sdk_soc.h``, then you can write application which
only use the resources of Nuclei Core.

If you want to use all the board resources, you can include the ``nuclei_sdk_hal.h``, then you can
write application for your own board, but the application can only run in the board you provided.

.. _design_peripheral_usage:

Usage
-----

If you want to learn about what peripheral driver you can use, you can check the ``nuclei_sdk_soc.h``
of each SoC, and ``nuclei_sdk_hal.h`` of each board.

**For SoC firmware library APIs:**

* You can find the **GD32VF103 SoC firmware library APIs** in ``SoC/gd32vf103/Common/Include``
* You can find the **HummingBird SoC firmware library APIs** in ``SoC/hbird/Common/Include``

If you just want to use SoC firmware library API, you just need to include ``nuclei_sdk_soc.h``,
then you can use the all the APIs in that SoC include directory.

.. note::

    For GD32VF103 SoC, if you want to use the USB driver API, then you need to add ``USB_DRV_SUPPORT = 1``
    in your application.


**For Board related APIs:**

* You can find the **GD32VF103 EVAL Board related APIs** in ``SoC/gd32vf103/Board/gd32vf103v_eval/Include``
* You can find the **GD32VF103 RV-STAR Board related APIs** in ``SoC/gd32vf103/Board/gd32vf103v_rvstar/Include``
* You can find the **Sipeed Longan Nano Board related APIs** in ``SoC/gd32vf103/Board/gd32vf103c_longan_nano/Include``
* You can find the **HummingBird EVAL Board related APIs** in ``SoC/hbird/Board/hbird_eval/Include``

If you just want to use all the APIs of Board and SoC, you just need to include ``nuclei_sdk_hal.h``,
then you can use the all the APIs in that Board and SoC include directory.
