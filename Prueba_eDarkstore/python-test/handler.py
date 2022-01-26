from fileinput import filename
from requests import get
from fpdf import FPDF
import json
import boto3
from botocore.client import Config


def llamada(event, context):
    response = get("https://mindicador.cl/api/uf")
    

    pdf = FPDF()
    pdf.add_page()

    pdf.set_font("Arial", size = 10)

    temp_response = json.dumps(response.json(),indent=4, separators=(". ", " = "))
    temp_response = temp_response.split('\n')
    for line in temp_response:
        pdf.cell(200, 10, txt = line, ln = 1, align = 'L')
    
    
    pdf.output("Data.pdf")  
    
    


    client = boto3.client('s3', aws_access_key_id='S3RVER', aws_secret_access_key='S3RVER',config = Config(s3={'addressing_style': 'path'}))

    client.upload_file(Filename="Data.pdf",Bucket="local-bucket",Key="Data.pdf")
    
    response = {"statusCode": 200, "body": response.text}
    
    return response