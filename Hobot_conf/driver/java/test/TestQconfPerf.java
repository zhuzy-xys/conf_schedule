import net.qihoo.hconf.Qconf;
import net.qihoo.hconf.QconfException;
import java.util.ArrayList;
public class TestQconfPerf
{

    public static void main(String[] args)
    {
        // ********************************Basic Usage******************************
        String key = "demo/conf";
        String idc = null;
        long beginTime = System.currentTimeMillis();
        for (int i = 0; i < 1000000; i++)
        {
            // Get Conf
            try
            {
                String value = Qconf.getHost(key, null);
            }
            catch(QconfException e)
            {
                e.printStackTrace();
            }
        }
        long endTime = System.currentTimeMillis();
        long costTime = endTime - beginTime;
        System.out.println("cost time :" + costTime);
    }
}


