#pragma once

//�����ṹ, ע�⵽ԭ�ṹΪ Field ��Ӧ�� Accessor ָ�� Field �������е�ƫ�ƶ����ഴ����ͬ�� Accessor ʵ��, ʵ���� Accessor �� Type ��һһ��Ӧ��
#define REFACTORING_0_TYPE_ACCESSOR_FIELD_RESTRUACTURING

#ifdef REFACTORING_0_TYPE_ACCESSOR_FIELD_RESTRUACTURING
	//��ʱ���δ���
	#define TEMP_DISABLED_FOR_REFACTORING_0
#endif

//���� std::array �� arg ����
#define USING_STD_ARRAY_TO_FILL_ARGS

//Ϊʵ�ֶ�̬���͵�ʵ����, ͨ�� alignof �ɱ�����ȷ��ע�����͵Ķ����С
//�ֽ�Ϊ������ʵ��, ���ڷ��ֿ����� VS2022 + v140(Qt5.8) �������޷��Գ�����ʹ�� alignof, �ݲ�����
//��ע, �˻�����δ���� _MSVC_TOOLSET_VERSION, _MSC_VER Ϊ 1942, �ޱ�׼���������Ƿ�Ϊ v140
//�������˽��ƽ̨����, �������� alignof ֧�����, ��ͨ�û�ȡ�������ٿ��Ǹ�Ϊ��ʽ֧��
//�������ʵ��, ���ú꼴��. ����ʾ�޷�ʵ����������Ĵ�����Ӧ����
//#define NIFLECTDEV_GENERATED_TYPE_ALIGNMENT