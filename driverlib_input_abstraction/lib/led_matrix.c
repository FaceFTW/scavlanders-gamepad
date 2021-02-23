/**********************************************************************
 * \headerfile: led_matrix.c
 * \author: Alex Westerman, Yifan Dai
 * Date Created: Feb 11, 2021
 * Description: Prototype header for lcd.c
 * \version: Feb 11, 2021
 ***********************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/***********************************************************************
 *                          FUNCTION DECLARATIONS                      *
***********************************************************************/#define ACCEL_PORT      GPIO_PORT_P1
const eUSCI_SPI_MasterConfig spiMasterConfig =
{ EUSCI_B_SPI_CLOCKSOURCE_SMCLK, 12000000, 1000000,
        EUSCI_B_SPI_MSB_FIRST,
        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH, EUSCI_B_SPI_3PIN };

const eUSCI_SPI_SlaveConfig spiSlaveConfig =
{ EUSCI_B_SPI_MSB_FIRST,
        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,
        EUSCI_B_SPI_3PIN
        };

/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(MSP_EXP432P401RLP_DMAControlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif

void start_LED_matrix(void){

    /* Configure CLK, MOSI & MISO for SPI0 (EUSCI_B0) */
        MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,
                GPIO_PIN5 | GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
        MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

        /* Configure SLAVE CLK, MOSI and SPMI (EUSCI_B2) */
        MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
                GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7,
                GPIO_PRIMARY_MODULE_FUNCTION);

        /* Configuring SPI module */
        MAP_SPI_initSlave(EUSCI_B2_BASE, &spiSlaveConfig);
        MAP_SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfig);

        /* Enable the SPI module */
        MAP_SPI_enableModule(EUSCI_B2_BASE);
        MAP_SPI_enableModule(EUSCI_B0_BASE);

        /* Configuring DMA module */
        MAP_DMA_enableModule();
        MAP_DMA_setControlBase(MSP_EXP432P401RLP_DMAControlTable);

        /* Assign DMA channel 0 to EUSCI_B0_TX0, channel 1 to EUSCI_B0_RX0 */
        MAP_DMA_assignChannel(DMA_CH0_EUSCIB0TX0);
        MAP_DMA_assignChannel(DMA_CH1_EUSCIB0RX0);
        MAP_DMA_assignChannel(DMA_CH4_EUSCIB2TX0);
        MAP_DMA_assignChannel(DMA_CH5_EUSCIB2RX0);

        /* Setup the TX transfer characteristics & buffers */
        MAP_DMA_setChannelControl(DMA_CH0_EUSCIB0TX0 | UDMA_PRI_SELECT,
        UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1);
        MAP_DMA_setChannelTransfer(DMA_CH0_EUSCIB0TX0 | UDMA_PRI_SELECT,
        UDMA_MODE_BASIC, mstxData,
                (void *) MAP_SPI_getTransmitBufferAddressForDMA(EUSCI_B0_BASE),
                MAP_SPI_MSG_LENGTH);

        /* Setup the RX transfer characteristics & buffers */
        MAP_DMA_setChannelControl(DMA_CH1_EUSCIB0RX0 | UDMA_PRI_SELECT,
        UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_1);
        MAP_DMA_setChannelTransfer(DMA_CH1_EUSCIB0RX0 | UDMA_PRI_SELECT,
        UDMA_MODE_BASIC,
                (void *) MAP_SPI_getReceiveBufferAddressForDMA(EUSCI_B0_BASE),
                msrxData,
                MAP_SPI_MSG_LENGTH);

        /* Slave Settings */
        MAP_DMA_setChannelControl(DMA_CH4_EUSCIB2TX0 | UDMA_PRI_SELECT,
        UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1);
        MAP_DMA_setChannelTransfer(DMA_CH4_EUSCIB2TX0 | UDMA_PRI_SELECT,
        UDMA_MODE_BASIC, sltxData,
                (void *) MAP_SPI_getTransmitBufferAddressForDMA(EUSCI_B2_BASE),
                MAP_SPI_MSG_LENGTH);

        /* Setup the RX transfer characteristics & buffers */
        MAP_DMA_setChannelControl(DMA_CH5_EUSCIB2RX0 | UDMA_PRI_SELECT,
        UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_1);
        MAP_DMA_setChannelTransfer(DMA_CH5_EUSCIB2RX0 | UDMA_PRI_SELECT,
        UDMA_MODE_BASIC,
                (void *) MAP_SPI_getReceiveBufferAddressForDMA(EUSCI_B2_BASE),
                slrxData,
                MAP_SPI_MSG_LENGTH);

        /* Enable DMA interrupt */
        MAP_DMA_assignInterrupt(INT_DMA_INT1, 1);
        MAP_DMA_clearInterruptFlag(DMA_CH1_EUSCIB0RX0 & 0x0F);

        /* Assigning/Enabling Interrupts */
        MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
        MAP_DMA_enableInterrupt(INT_DMA_INT1);
        MAP_DMA_enableChannel(5);
        MAP_DMA_enableChannel(4);

        /* Delaying for forty cycles to let the master catch up with the slave */
        for(ii=0;ii<50;ii++);

        MAP_DMA_enableChannel(1);
        MAP_DMA_enableChannel(0);

        /* Polling to see if the master receive is finished */
        while (1)
        {
            if (isrCounter > 0)
            {
                __no_operation();
            }
        }


}

void DMA_INT1_IRQHandler(void)
{
    isrCounter++;
    MAP_DMA_clearInterruptFlag(0);
    MAP_DMA_clearInterruptFlag(1);

    /* Disable the interrupt to allow execution */
    MAP_Interrupt_disableInterrupt(INT_DMA_INT1);
    MAP_DMA_disableInterrupt(INT_DMA_INT1);
}






