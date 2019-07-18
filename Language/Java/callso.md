# Java调用Linux动态链接库

话不多说，直接上代码

- 注意`native`关键字
- 为什么要把.so文件转写入一个临时的.so文件再去读？以后用到可以研究一下

```java
public class PPLogStatApi{

    // 静态初始化块
	static {
		try {
			String file_so = "";
			if ( System.getProperty("sun.arch.data.model").equals("64") ){
				file_so = "libpropertyplus-64.so";
			}else{
				file_so = "libpropertyplus-32.so";
			}

			InputStream in = PPLogStatApi.class.getResourceAsStream(file_so);
			File so = File.createTempFile("libpropertyplus", ".so");
			FileOutputStream out = new FileOutputStream(so);
			int i;
			byte[] buf = new byte[1024];
			while( (i=in.read(buf) )!=-1){
				out.write(buf,0,i);
			}
			in.close();
			out.close();
			System.load(so.toString());

			so.delete();

			//System.out.println("libpropertyplus init ok");
		} catch (Exception e) {
			System.out.println(e.toString());
		}
	}


	public native static void log(String appname, String[] keys, float[] values);

    public native static void setAppName(String name);
	
	public native static void show_debug_log();
}