import requests
import time
import sys

def DownloadFile(url, filepath):
    with open(filepath, 'wb') as f:
        print('Waiting for response...')
        response = requests.get(url, stream=True)
        total = int(response.headers.get('content-length'))
        print('Downloading...')
        if total is None or total < 0:
            print("File size is unknown or invalid. Downloading without progress indication.")
            f.write(response.content)
        else:
            downloaded = 0
            startTime = time.time()
            for data in response.iter_content(chunk_size=max(int(total/1000), 1024*1024)):
                downloaded += len(data)
                f.write(data)
                done = int(50*downloaded/total)
                percentage = (downloaded / total) * 100
                elapsedTime = time.time() - startTime
                avgKBPerSecond = (downloaded / 1024) / elapsedTime
                avgSpeedString = '{:.2f} KB/s'.format(avgKBPerSecond)
                if (avgKBPerSecond > 1024):
                    avgMBPerSecond = avgKBPerSecond / 1024
                    avgSpeedString = '{:.2f} MB/s'.format(avgMBPerSecond)
                sys.stdout.write('\r[{}{}] {:.2f}% ({})     '.format('█' * done, '.' * (50-done), percentage, avgSpeedString))
                sys.stdout.flush()
    sys.stdout.write('\n')

def AskYesOrNo(prompt):
    while True:
        response = input(prompt + " (yes/no): ").strip().lower()
        if response in ['yes', 'y']:
            return True
        elif response in ['no', 'n']:
            return False
        else:
            print("Please respond with 'yes' or 'no'.")

