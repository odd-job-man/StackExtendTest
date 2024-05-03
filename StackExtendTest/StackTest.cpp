#include <stdio.h>
#include <Windows.h>

#define kSTACKSIZE (1024) * (1024)
#define kPAGESIZE (4) * (1024)

int main()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery(&si, &mbi, sizeof(mbi));
	// ������ 1MB ���ϴ� �ּҸ� ����.
	UINT_PTR pStackEnd = (UINT_PTR)mbi.AllocationBase+ kSTACKSIZE;
	BYTE* pGuard = nullptr;

	while (true)
	{
		UINT_PTR pPosition = (UINT_PTR)mbi.AllocationBase;
		while (pPosition< pStackEnd)
		{
			SIZE_T size = VirtualQuery((LPCVOID)pPosition, &mbi, sizeof(mbi));
			if (!size) __debugbreak();

			printf("addr : %x, region size : %d KBytes\n", pPosition, mbi.RegionSize / 1024);
			printf("Page Protection : ");
			if (mbi.Protect & PAGE_READWRITE)
				printf("PAGE_READWRITE ");
			if (mbi.Protect & PAGE_GUARD)
				printf("PAGE_GUARD ");
			printf("\n");

			printf("Virtual Memory Allocation State : ");
			switch (mbi.State)
			{
			case MEM_COMMIT:
				printf("MEM_COMMIT ");
				break;
			case MEM_RESERVE:
				printf("MEM_RESERVE");
				break;
			default:
				printf("MEM_FREE");
				break;
			}
			printf("\n\n");
			pPosition += mbi.RegionSize;
		}
		printf("--------------------------------------\n");

		// ������ ������ �޸𸮻� ���ϴ��� ������������ �ֻ�� �ּҸ� ����
		pGuard = (BYTE*)(pPosition- mbi.RegionSize - kPAGESIZE);
		__try
		{
			// ���� �ּ��� ������������ �ǵ���� ���� �Ӽ� �����ϰ� ������������ ������ �����
			*pGuard = 1;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			if (GetExceptionCode() == EXCEPTION_STACK_OVERFLOW)
			{
				printf("Stack OverFlow Occur");
				return 0;
			}
			else
			{
				__debugbreak();
			}
		}
	}
}