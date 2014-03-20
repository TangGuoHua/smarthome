
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;

namespace COMUtil
{
    public partial class formMain : Form
    {
        private SerialPort _sPort = new SerialPort();
        private int _status = 0;
        private int[] _hit = new int[17];
        private int[] _hitTotal = new int[17];
        private int _hitIndex = 0;

        private DateTime _t0, _t1, _overallT0;
        private double _totalTxTime = 0;

        private int _batchCount = 0;
        private int _maxBatchcount = 0;

        private const int PACKAGE_COUNT_PER_BATCH = 250;

        public formMain()
        {
            InitializeComponent();
        }

        private void btnOpenClose_Click(object sender, EventArgs e)
        {
            if (btnOpenClose.Text == "Open")
            {
                try
                {
                    _sPort.PortName = comboPort.Text;
                    _sPort.BaudRate = int.Parse(comboBaudRate.Text);
                    _sPort.Open();
                    _sPort.DataReceived += new SerialDataReceivedEventHandler(sPort_DataReceived);
                    btnOpenClose.Text = "Close";

                    _overallT0 = DateTime.Now;
                    _totalTxTime = 0;
                    _batchCount = 0;
                    switch (comboBatchCount.Text)
                    {
                        case "5 Batches":
                            _maxBatchcount = 5;
                            break;
                        case "10 Batches":
                            _maxBatchcount = 10;
                            break;
                        case "20 Batches":
                            _maxBatchcount = 20;
                            break;
                        case "50 Batches":
                            _maxBatchcount = 50;
                            break;
                        case "100 Batches":
                            _maxBatchcount = 100;
                            break;
                        case "999 Batches":
                            _maxBatchcount = 999;
                            break;
                    }

                    for (int i = 0; i <= 16; i++)
                    {
                        _hitTotal[i] = 0;
                    }

                    appendLog("==============\r\n");
                    appendLog(string.Format("Start testing for '{0}' ({1} packages per batch) on {2}.\r\n", comboBatchCount.Text, PACKAGE_COUNT_PER_BATCH, DateTime.Now.ToString("HH:mm:ss.fff")));

                }
                catch (Exception)
                {

                    MessageBox.Show("Open port failed.");
                }
            }
            else
            {
                closeSerialPort();

                btnOpenClose.Text = "Open";

                printOverallResult();
            }



        }

        private void printOverallResult()
        {
            DateTime tmpT1 = DateTime.Now;
            TimeSpan elapsed = tmpT1.Subtract(_overallT0 );

            appendLog(string.Format("Testing ends on {0}.\r\nTime elapsed {1}.", tmpT1.ToString("HH:mm:ss.fff"), elapsed.ToString("c")));
            appendLog(string.Format("\r\n\r\nOverall results for {1} batches ({0} packages) are:", PACKAGE_COUNT_PER_BATCH * _batchCount, _batchCount));

            appendLog(string.Format("\r\n  Avg. Batch Tx-time:{0:0}ms", _totalTxTime / _batchCount ));
            appendLog(string.Format("  FTR:{0:0%}", (double)_hitTotal[0] / (PACKAGE_COUNT_PER_BATCH * _batchCount)));
            appendLog(string.Format("  U5:{0:0%}", (double)(_hitTotal[0] + _hitTotal[1] + _hitTotal[2] + _hitTotal[3] + _hitTotal[4] + _hitTotal[5]) / (PACKAGE_COUNT_PER_BATCH * _batchCount)));
            appendLog(string.Format("  Re-Tx:{0:0%}", (double)(PACKAGE_COUNT_PER_BATCH * _batchCount - _hitTotal[0] - _hitTotal[16]) / (PACKAGE_COUNT_PER_BATCH * _batchCount)));
            appendLog(string.Format("  Loss:{0:0%}", (double)_hitTotal[16] / (PACKAGE_COUNT_PER_BATCH * _batchCount)));
            appendLog("\r\n\r\n");
        }

        void sPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {

            int readBuffSize = _sPort.BytesToRead;

            if (readBuffSize > 0)
            {
                //Console.Out.Write( "\nbuff size={0}  ", buffSize );
                byte[] buff = new byte[readBuffSize]; //声明一个临时数组存储当前来的串口数据 
                _sPort.Read(buff, 0, readBuffSize); //读取缓冲数据 

                foreach (byte b in buff)
                {
                    //appendLog(string.Format("r:{0} s:{1}  ", b, _status));
                    switch (_status)
                    {
                        case 0:

                            if (b == 0xff)
                            {
                                // nrf24l01传送开始
                                _t0 = DateTime.Now;
                                _status = 1;
                            }

                            break;

                        case 1:
                            if (b == 0xfe)
                            {
                                // nrf24l01传送结束。开始通过串口传送结果
                                _t1 = DateTime.Now;
                                _status = 2;
                                _hitIndex = 0;

                                for (int i = 0; i <= 16; i++)
                                    _hit[i] = 0;

                            }


                            break;

                        case 2:
                            if (b == 0xfd)
                            {

                                //收到0xfd 
                                //一个batch结束
                                _status = 0;

                                double tmpTxTime = _t1.Subtract(_t0).TotalMilliseconds;
                                _totalTxTime += tmpTxTime;
                                ++_batchCount;
                                appendLog(DateTime.Now.ToString("HH:mm:ss.fff"));
                                appendLog(string.Format(" Batch #{0:000}", _batchCount));
                                appendLog(string.Format(" Tx-time:{0:00000}ms", tmpTxTime));

                                appendLog(string.Format("  FTR:{1:000.0%}({0:000})", _hit[0], (double)_hit[0] / PACKAGE_COUNT_PER_BATCH));
                                appendLog(string.Format("  U5:{1:000.0%}({0:000})", _hit[0] + _hit[1] + _hit[2] + _hit[3] + _hit[4] + _hit[5], (double)(_hit[0] + _hit[1] + _hit[2] + _hit[3] + _hit[4] + _hit[5]) / PACKAGE_COUNT_PER_BATCH));
                                appendLog(string.Format("  Re-Tx:{1:000.0%}({0:000})", PACKAGE_COUNT_PER_BATCH - _hit[0] - _hit[16], (double)(PACKAGE_COUNT_PER_BATCH - _hit[0] - _hit[16]) / PACKAGE_COUNT_PER_BATCH));
                                appendLog(string.Format("  Loss:{1:000.0%}({0:000})", _hit[16], (double)_hit[16] / PACKAGE_COUNT_PER_BATCH));

                                for (int i = 0; i <= 16; i++)
                                {
                                    _hitTotal[i] += _hit[i];
                                }
                                appendLog("\n");

                                if (_batchCount >= _maxBatchcount)
                                {
                                    //btnOpenClose_Click(null, null);
                                    closeSerialPort();
                                    

                                    btnOpenClose.Invoke(new MethodInvoker(
                                        delegate
                                        {
                                            btnOpenClose.Text = "Open";
                                        }
                                        ));
                                    printOverallResult();
                                }
                            }
                            else
                            {
                                _hit[_hitIndex++] = b;
                            }
                            break;
                    }
                }
                //appendLog(readBuffSize);
            }
        }

        private void closeSerialPort()
        {
            _sPort.DataReceived -= sPort_DataReceived;
            _sPort.DiscardInBuffer();
            _sPort.Close();
        }

        private void appendLog(string txt)
        {
            txtLog.Invoke(new MethodInvoker(
                            delegate
                            {
                                txtLog.AppendText(txt);
                                //string.Format("{0} {1} bytes received,\n",
                                //                DateTime.Now.ToString("HH:mm:ss.fff"),
                                //                readBuffSize

                                //    ));
                            }
                            ));
        }

        private void btnClearLog_Click(object sender, EventArgs e)
        {
            txtLog.Clear();
        }

        private void formMain_Load(object sender, EventArgs e)
        {

        }
    }
}
