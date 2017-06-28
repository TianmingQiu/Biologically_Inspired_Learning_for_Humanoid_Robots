clear ;
clc;
close all ;

for i = 1 : 20 
    
    s1 = '/Users/xujiachen/Desktop/data/Q0_' ;
    s2 = num2str(i-1) ;
    s3 = '.txt' ;
    s= [s1,s2,s3] ;
    Q0(:,:,i) = load(s);
end

for i = 1 : 30 
    
    s1 = '/Users/xujiachen/Desktop/data/Q1_' ;
    s2 = num2str(i-1) ;
    s3 = '.txt' ;
    s= [s1,s2,s3] ;
    Q1(:,:,i) = load(s);
end

for i = 1 : 34
    
    s1 = '/Users/xujiachen/Desktop/data/Q2_' ;
    s2 = num2str(i-1) ;
    s3 = '.txt' ;
    s= [s1,s2,s3] ;
    Q2(:,:,i) = load(s);
end

for i = 1 : 21 
    
    s1 = '/Users/xujiachen/Desktop/data/Q3_' ;
    s2 = num2str(i-1) ;
    s3 = '.txt' ;
    s= [s1,s2,s3] ;
    Q3(:,:,i) = load(s);
end

for i = 1 : 24
    
    s1 = '/Users/xujiachen/Desktop/data/Q4_' ;
    s2 = num2str(i-1) ;
    s3 = '.txt' ;
    s= [s1,s2,s3] ;
    Q4(:,:,i) = load(s);
end

cumu_reward_plot(Q0,0)
cumu_reward_plot(Q1,1)
cumu_reward_plot(Q2,2)
cumu_reward_plot(Q3,3)
cumu_reward_plot(Q4,4)
function cumu_reward_plot(Q,no)
len = size(Q,3);
figure

for j = 1 : 5
    flag = 1 ;
    s = [ '51', num2str(j)];
    subplot(s)
    if j==1
        x = ['Goalkeeper Position: x1[',num2str(no),']'] ;
        title (x) ;
        hold on;
    end
    for i = 1 : 3
        
        switch i
            case 1
                text = 'r' ;
            case 2
                text = 'b' ;
            case 3
                text = 'g' ;
        end
        if ( ( (i==2)&&(j==1) ) || ( (i==3)&&(j==5) ) )
            flag = 0 ;
        end
        
        
        
        a = reshape(Q(i,j,:),[1 len]) ;
        if( flag == 1 )
            plot(1:len , a ,text);
        end
        
        hold on ;


    end
    

        
    y = ['x2[',num2str(j-1),']'];
    ylabel(y);
    legend('kick','in','out')
    hold on
    if (j == 1)
        a = reshape(Q(3,j,:),[1 len]) ;
        plot(1:len , a ,text);
        hold on;
        legend('kick','out')
    end

    
end
axis([ 1 (len+2) -30 90])
xlabel('number of actions') ;

hold on;

end
