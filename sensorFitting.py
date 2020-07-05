import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import os

dataPath = os.path.join(os.path.dirname(__file__), 'sensorTest.xlsx')


def fetchData(sheetName):
    df = pd.read_excel(dataPath, sheet_name=sheetName)
    print(df)
    sensorOutput = np.array(df['Sensor Value'])
    midiAssignment = np.array(df['MIDI Value'])
    return sensorOutput, midiAssignment

def fitPoly(sensorValues, midiValues):
    return np.polyfit(sensorValues, midiValues, deg=5)


def compareFitToData(fitParams, sensorValues, midiValues):
    plt.plot(sensorValues, midiValues, 'o', label='Raw Data')
    sensorRange = np.linspace(0, 1024, 100)
    def mapSensor(sensorVal):
        x = sensorVal
        sum = 0
        for i in range(len(fitParams)):
            sum += fitParams[i] * x ** (len(fitParams) - i - 1)
        return sum
    plt.plot(sensorValues, mapSensor(sensorValues), label='Fit')
    plt.legend()
    plt.show()


if __name__ == '__main__':
    sensorValues, midiValues = fetchData('Strum')
    fitParams = fitPoly(sensorValues, midiValues)
    compareFitToData(fitParams, sensorValues, midiValues)
    print('Params = {}'.format(fitParams))
