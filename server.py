import streamlit as st
import os

LOG_FILE = 'temperature_log.txt'


def get_last_value():
    if os.path.exists(LOG_FILE):
        with open(LOG_FILE, 'r') as file:
            lines = file.readlines()
            if lines:
                last_line = lines[-1]
                timestamp, value = last_line.split(': ')
                return timestamp, value.strip()
    return None, None


st.title("Temperature Sensor Data")
timestamp, last_value = get_last_value()

if last_value:
    st.write(f"Last recorded temperature: {last_value}°C at {timestamp}")
else:
    st.write("No data available.")
